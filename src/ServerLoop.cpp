/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/04/27 17:11:28 by asalo            ###   ########.fr       */
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

void ServerLoop::stopServer() {
    _run = false;
}
void ServerLoop::stop() {
    _run = false;
}

bool ServerLoop::hasTimedOut() {
    time_t currentTime = time(nullptr);
    return (currentTime - _startUpTime) >= 1800;
}

/**
 * @brief   Initializes and configs listening sockets for each port in the ServerBlocks
 *          and adds them to the poll structure.
 */
void ServerLoop::setupServerSockets() {
    for (std::vector<ServerBlock>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
        const std::vector<int>& ports = it->getPorts();

        for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket < 0) {
                Logger::getInstance().logLevel("SYSTEM", "Failed to create socket for port " + std::to_string(*portIt), 1);
                continue ;
            }

            int opt = 1;
            if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                Logger::getInstance().logLevel("SYSTEM", "Failed to set socket options for port " + std::to_string(*portIt), 1);
                close(serverSocket);
                continue ;
            }
            struct sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(*portIt);

            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            if (inet_pton(AF_INET, it->getHost().c_str(), &serverAddr.sin_addr) <= 0) {
                Logger::getInstance().logLevel("SYSTEM", "Invalid address for port " + std::to_string(*portIt), 1);
                close(serverSocket);
                continue ;
            }
            if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                Logger::getInstance().logLevel("SYSTEM", "Failed to bind socket for port " + std::to_string(*portIt), 1);
                close(serverSocket);
                continue ;
            }
            if (listen(serverSocket, SOMAXCONN) < 0) {
                Logger::getInstance().logLevel("SYSTEM", "Failed to listen on port " + std::to_string(*portIt), 1);
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
            logStream << "Server loop started (" << it->getServerName() << "), listening on port " << *portIt << " (fd: " << serverSocket << ")";
            Logger::getInstance().logLevel("SYSTEM", logStream.str(), 0);
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
        std::cerr << GC "clients: " << _clients.size() << RES << std::endl;
        return true;
     }
    return false;
}

/**
 * @brief   Accepts a new client connection on a listening socket and sets it up (non blocking, session info).
 */
void ServerLoop::acceptNewConnection(int serverSocket) {
    if (serverFull())
        return ;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientFd < 0) {
        Logger::getInstance().logLevel("SYSTEM", "Failed to accept client connection.", 1);
        return ;
    }

    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0) {
        Logger::getInstance().logLevel("SYSTEM", "Failed to set client socket non-blocking", 1);
        close(clientFd);
        return ;
    }
    struct sockaddr_in localAddr; // Retrieve the local (server) port for the accepted connection.
    socklen_t localLen = sizeof(localAddr);
    if (getsockname(clientFd, (struct sockaddr*)&localAddr, &localLen) < 0) {
        Logger::getInstance().logLevel("SYSTEM", "Failed to get local address for client connection.", 1);
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

    ClientSession session(clientFd); // Create and store the client session with the correct ServerBlock.
    session._block = _portToBlock[localPort]; // Assign correct ServerBlock
    session.setServerName(_portToBlock[localPort].getServerName()); // Set the server_name
    _clients[clientFd] = session;
    // std::cout << REV_WHITE << session.getServerName() << RES << std::endl;

    struct pollfd pfd; // Add the new client to the poll vector.
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);
    _clientLastActivity[clientFd] = time(nullptr); // Client activity for time-outs

    std::ostringstream logStream;
    char clientIp[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, INET_ADDRSTRLEN);
    logStream << "New client connected from " << clientIp << " on port " << localPort << " (fd: " << clientFd << ")";
    Logger::getInstance().logLevel("INFO", logStream.str(), 0);
}

/**
 * @brief   Reads data from a client and sends incoming requests for parsing and processing.
 */
void ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[4096]; // For each recv
    ssize_t bytesRead;
    HttpParser parser(_clients[clientSocket]._block.getBodySize());
    Logger::getInstance().checkErrorPages(_clients[clientSocket]._block);

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        _clients[clientSocket].buffer.append(buffer, bytesRead);
        if (parser.isFullRequest(_clients[clientSocket].buffer, bytesRead))
            break ;
        if (parser.getState() != 0) {
            _clients[clientSocket].buffer.clear();
            sendResponse(clientSocket, Logger::getInstance().logLevel("ERROR", "", parser.getState()));
            removeClient(clientSocket);
            return ;
        }
    }
    if (bytesRead == 0) {
        removeClient(clientSocket);
        return ;
    } else if (bytesRead < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return ;
        }
        removeClient(clientSocket);
        std::ostringstream logStream;
        logStream << "Unable to read from socket: " << clientSocket;
        Logger::getInstance().logLevel("INFO", logStream.str(), 0);
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
                Logger::getInstance().logLevel("SYSTEM", logStream.str(), 0);
            }
        }
    }
    if (_clients[clientSocket].requestLimiter()) {
        sendResponse(clientSocket, Logger::getInstance().logLevel("ERROR", "", 429));
        return ;
    }
    if (parser.parseRequest(_clients[clientSocket]._block)) {
        _clients[clientSocket].request = parser.getPendingRequest();
        std::string response = Router::getInstance().routeRequest(_clients[clientSocket].request);
        sendResponse(clientSocket, response);
        // close(clientSocket);
        removeClient(clientSocket);
    } else {
        int state = parser.getState();
        if (state == 301 || state == 302)
            sendResponse(clientSocket, Logger::getInstance().logLevel("REDIR", parser.getRedirection(), parser.getState()));
        else
            sendResponse(clientSocket, Logger::getInstance().logLevel("ERROR", "", parser.getState()));
        removeClient(clientSocket);
    }
    Logger::getInstance().resetErrorPages();
}

void    ServerLoop::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        Logger::getInstance().logLevel("ERROR", "Failed to send response to client.", 1);
        std::string errorResponse = INTERNAL + Logger::getInstance().logLevel("ERROR", "Internal Server Error", 500);
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

/**
 * @brief   Main Server Loop. Uses poll to monitor sockets.
 */
void ServerLoop::startServer() {
    try {
        setupServerSockets();
        if (!_run) {
            Logger::getInstance().logLevel("ERROR", "Server setup failed. Cannot start loop.", 0);
            return ;
        }
    } catch (const std::exception& e) {
        Logger::getInstance().logLevel("ERROR", "Exception during server setup: " + std::string(e.what()), 1);
        return ;
    }

    _startUpTime = time(nullptr);

    while (_run) {
        int pollResult = poll(_pollFds.data(), _pollFds.size(), 1000);
        if (pollResult < 0) {
            Logger::getInstance().logLevel("SYSTEM", "poll() interrupted.", 0);
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
    // Map of fd's to ServerBlock for server sockets
    std::map<int, ServerBlock> fdToBlock;
    size_t socketIndex = 0; // Track pos in _serverSockets & _boundPorts
    for (std::vector<ServerBlock>::iterator blockIt = _serverBlocks.begin(); blockIt != _serverBlocks.end(); ++blockIt) {
        const std::vector<int>& ports = blockIt->getPorts();
        for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
            if (socketIndex < _serverSockets.size() && socketIndex < _boundPorts.size() && _boundPorts[socketIndex] == *portIt) {
                fdToBlock[_serverSockets[socketIndex]] = *blockIt;
                ++socketIndex;
            }
        }
    }

    // Iterate through _pollFds & close close them
    for (std::vector<struct pollfd>::const_iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        std::string serverName = "unknown";
        int fd = it->fd;

        // Is it a server socket?
        std::map<int, ServerBlock>::const_iterator fdIt = fdToBlock.find(fd);
        if (fdIt != fdToBlock.end()) {
            serverName = fdIt->second.getServerName();
        } else {
            // Is it a client socket?
            std::map<int, ClientSession>::const_iterator clientIt = _clients.find(fd);
            if (clientIt != _clients.end()) {
                serverName = clientIt->second._block.getServerName();
            }
        }

        std::ostringstream responseStream;
        responseStream << "Terminating server loop (" << serverName << ")";
        Logger::getInstance().logLevel("SYSTEM", responseStream.str(), 0);
        close(fd);
    }
    _pollFds.clear();
    Logger::getInstance().logLevel("SYSTEM", "Resources cleaned.", 0);
}