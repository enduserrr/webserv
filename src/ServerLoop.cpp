/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/04/03 10:40:21 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ServerLoop.hpp"
#include "StaticHandler.hpp"
#include "Router.hpp"
#include "ServerBlock.hpp"

ServerLoop::ServerLoop() {
    _run = true;
}

ServerLoop::ServerLoop(const std::vector<ServerBlock> &serverBlocks)
    : _serverBlocks(serverBlocks) {
        _run = true;
}

ServerLoop::~ServerLoop() {
    closeServer();
}



void ServerLoop::setupServerSockets() {
    for (std::vector<ServerBlock>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
        const std::vector<int>& ports = it->getPorts();

        for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
            if (std::find(_boundPorts.begin(), _boundPorts.end(), *portIt) != _boundPorts.end()) {
                continue ; // Skip duplicate port binding
            }

            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket < 0) {
                Logger::getInstance().logLevel("SYS_ERROR", "Failed to create socket for port " + std::to_string(*portIt), 1);
                continue ;
            }

            int opt = 1;
            if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                Logger::getInstance().logLevel("SYS_ERROR", "Failed to set socket options for port " + std::to_string(*portIt), 1);
                close(serverSocket);
                continue ;
            }
            struct sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(*portIt);
            // // Bind to 0.0.0.0 to accept connections on all interfaces, not just localhost
            // serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            // // if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) { // Original binding to localhost only
            // //     Logger::getInstance().logLevel("SYS_ERROR", "Invalid address for port " + std::to_string(*portIt), 1);
            // //     close(serverSocket);
            // //     continue ;
            // // }
            if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
                Logger::getInstance().logLevel("SYS_ERROR", "Invalid address for port " + std::to_string(*portIt), 1);
                close(serverSocket);
                continue ;
            }
            if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                Logger::getInstance().logLevel("SYS_ERROR", "Failed to bind socket for port " + std::to_string(*portIt), 1);
                close(serverSocket);
                continue ;
            }
            if (listen(serverSocket, SOMAXCONN) < 0) {
                Logger::getInstance().logLevel("SYS_ERROR", "Failed to listen on port " + std::to_string(*portIt), 1);
                close(serverSocket);
                continue ;
            }
            struct pollfd pfd;
            pfd.fd = serverSocket;
            pfd.events = POLLIN;
            _pollFds.push_back(pfd);
            _serverSockets.push_back(serverSocket);
            _boundPorts.push_back(*portIt);
            _portToBlock[*portIt] = *it;
            std::ostringstream logStream;
            logStream << "Server listening on port: " << *portIt;
            Logger::getInstance().logLevel("INFO", logStream.str(), 0);
        }
    }
    if (_serverSockets.empty()) {// Meaby an exception instead
        Logger::getInstance().logLevel("ERROR", "No server sockets were successfully set up. Exiting.", 1);
        _run = false;
    }
}

bool ServerLoop::serverFull() {
     if (_clients.size() >= MAX_CLIENTS) {
        Logger::getInstance().logLevel("INFO", "Server if full! Rejecting new clients", 0);
        std::cerr << "clients: " << _clients.size()<< std::endl;
        return true;
     }
    return false;
}

void ServerLoop::acceptNewConnection(int serverSocket) {
    if (serverFull()) // Add logLevel call
        return ;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientFd < 0) {
        Logger::getInstance().logLevel("SYS_ERROR", "Failed to accept client connection.", 1);
        return ;
    }

    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
        Logger::getInstance().logLevel("SYS_ERROR", "Failed to set client socket non-blocking", 1);
        close(clientFd);
        return ;
    }
    struct sockaddr_in localAddr; // Retrieve the local (server) port for the accepted connection.
    socklen_t localLen = sizeof(localAddr);
    if (getsockname(clientFd, (struct sockaddr*)&localAddr, &localLen) < 0) {
        Logger::getInstance().logLevel("SYS_ERROR", "Failed to get local address for client connection.", 1);
        close(clientFd);
        return ;
    }
    int localPort = ntohs(localAddr.sin_port);
    if (_portToBlock.find(localPort) == _portToBlock.end()) {// Find ServerBlock for this port
        std::ostringstream logStream;
        logStream << "No ServerBlock found for port: " << localPort;
        Logger::getInstance().logLevel("WARNING", logStream.str(), 1);
        close(clientFd);
        return ;
    }

    ClientSession session(clientFd);// Create and store the client session with the correct ServerBlock.
    session._block = _portToBlock[localPort]; //Assign correct ServerBlock
    _clients[clientFd] = session;

    struct pollfd pfd;// Add the new client to the poll vector.
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);
    _clientLastActivity[clientFd] = time(nullptr); //Client activity for time-outs

    std::ostringstream logStream;
    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
    logStream << "New client connected from " << clientIp << " on port " << localPort  << " (fd: " << clientFd << ")";
    Logger::getInstance().logLevel("INFO", logStream.str(), 0);
}

/**
 * @brief   Reads and accumulates received data into a given clients buffer
 *          and determines whether received request is single or multi part.
 */
void ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[4096]; // For each recv
    ssize_t bytesRead;
    HttpParser parser(_clients[clientSocket]._block.getBodySize());

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        _clients[clientSocket].buffer.append(buffer, bytesRead);
        if (parser.isFullRequest(_clients[clientSocket].buffer, bytesRead))
            break ;
        if (parser.getState() != 0) {
            sendResponse(clientSocket, std::string("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n") +
            Logger::getInstance().logLevel("ERROR", "Bad Request", parser.getState()));
            removeClient(clientSocket);
            return ;
        }
    }
    if (bytesRead == 0) {
        removeClient(clientSocket);
        return ;
    } else if (bytesRead < 0) {
        std::ostringstream logStream;
        logStream << "Unable to read from socket: " << clientSocket  << ": " << strerror(errno);
        Logger::getInstance().logLevel("WARNING", logStream.str(), 0);
        removeClient(clientSocket);
        return ;
    }
    if (_clients.find(clientSocket) == _clients.end()) {// Check if client session exists
        _clients[clientSocket] = ClientSession(clientSocket);
        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        if (getsockname(clientSocket, (struct sockaddr*)&addr, &addrLen) == 0) {
            int port = ntohs(addr.sin_port);
            if (_portToBlock.find(port) != _portToBlock.end()) {
                _clients[clientSocket]._block = _portToBlock[port];
            } else {
                std::ostringstream logStream;
                logStream << "No matching ServerBlock for port: " << port;
                Logger::getInstance().logLevel("WARNING", logStream.str(), 0);
            }
        }
    }
    if (_clients[clientSocket].requestLimiter()) {
        sendResponse(clientSocket, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n" +
                    Logger::getInstance().logLevel("ERROR", "Client request limit reached.", 429));
        return ;
    }
    if (parser.parseRequest(_clients[clientSocket]._block)) {
        _clients[clientSocket].request = parser.getPendingRequest();
        std::string response = Router().routeRequest(_clients[clientSocket].request, clientSocket);
        sendResponse(clientSocket, response);
        removeClient(clientSocket);
    } else {
        sendResponse(clientSocket, std::string("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n") +
        Logger::getInstance().logLevel("ERROR", "Bad Request", parser.getState()));
        removeClient(clientSocket);
    }
}

void    ServerLoop::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        Logger::getInstance().logLevel("SYS_ERROR", "Failed to send response to client.", 1);
        std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                                    + Logger::getInstance().getErrorPage(500);
        send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
    }
    return ;
}

void ServerLoop::checkClientTimeouts() {
    time_t currentTime = time(nullptr);
    std::vector<int> fdsToRemove;

    for (std::map<int, time_t>::const_iterator it = _clientLastActivity.begin(); it != _clientLastActivity.end(); ++it) {
        int fd = it->first;
        time_t lastActivityTime = it->second;

        if ((currentTime - lastActivityTime) >= _clientTimeoutDuration) {
            std::ostringstream logStream;
            logStream << "Client fd " << fd << " timed out after "
                      << (currentTime - lastActivityTime) << "s. Closing.";
            Logger::getInstance().logLevel("INFO", logStream.str(), 0);
            fdsToRemove.push_back(fd);
        }
    }
    // Remove timed-out clients
    for (std::vector<int>::iterator it = fdsToRemove.begin(); it != fdsToRemove.end(); ++it) {
        removeClient(*it);
    }
}

void ServerLoop::startServer() {
    try {
        setupServerSockets();
        if (!_run) {
            Logger::getInstance().logLevel("ERROR", "Server setup failed. Cannot start loop.", 0);
            return ;
        }
    } catch (const std::exception& e) {
        Logger::getInstance().logLevel("FATAL", "Exception during server setup: " + std::string(e.what()), 1);
        return ;
    }

    _startUpTime = time(nullptr);
    Logger::getInstance().logLevel("INFO", "Server main loop started.", 0);

    while (_run) {
        int pollResult = poll(_pollFds.data(), _pollFds.size(), 2000);
        if (pollResult < 0) {
            Logger::getInstance().logLevel("SYS_ERROR", "Fatal error in poll()", 1);
            _run = false;
            break ;
        }

        time_t currentTime = time(nullptr); // Get time once per loop iteration

        // Process ready file descriptors
        size_t currentPollFdsCount = _pollFds.size();
        for (size_t i = 0; i < currentPollFdsCount; ++i) {
            // removeClient can alter _pollFds
            if (i >= _pollFds.size()) {
                 break ;
            }

            struct pollfd& currentPfd = _pollFds[i];
            int fd = currentPfd.fd;

            // Check ONLY for incoming data/connections (POLLIN)
            if (currentPfd.revents & POLLIN) {
                // Determine if it's a listening server socket or a client socket
                bool isServer = false;
                for(size_t s = 0; s < _serverSockets.size(); ++s) {
                    if (_serverSockets[s] == fd) {
                        isServer = true;
                        break ;
                    }
                }
                if (isServer) {
                    acceptNewConnection(fd);
                } else {
                    _clientLastActivity[fd] = currentTime;
                    handleClientRequest(fd);
                    // Check if handleClientRequest removed the client
                    bool clientStillExists = (_clients.find(fd) != _clients.end());
                    if (!clientStillExists) {
                        currentPollFdsCount--;
                        i--;
                    }
                }
            }
        }
        if (_run) {
            checkClientTimeouts();
        }
    }
    Logger::getInstance().logLevel("INFO", "Server loop terminated.", 0);
    closeServer();
}

void ServerLoop::removeClient(int clientFd) {
    std::map<int, ClientSession>::iterator it = _clients.find(clientFd);
    if (it != _clients.end()) {
        close(clientFd); // Close client socket descriptor
        _clients.erase(it); // Erase client from map
        _clientLastActivity.erase(clientFd); // Erase from activity map
        _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
                        [clientFd](struct pollfd& pfd) { return pfd.fd == clientFd; }),
                        _pollFds.end());
        std::ostringstream logStream;
        logStream << "Client disconnected (fd: " << clientFd << ")";
        Logger::getInstance().logLevel("INFO", logStream.str(), 0);
    } else {
         Logger::getInstance().logLevel("WARNING", "Tried to remove non-existent client fd: " + std::to_string(clientFd), 0);
    }
}

void ServerLoop::closeServer() {
    for (std::vector<struct pollfd>::const_iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        close(it->fd);
    }
    _pollFds.clear();
    Logger::getInstance().logLevel("INFO", "Server closed and resources cleaned.", 0);
}

