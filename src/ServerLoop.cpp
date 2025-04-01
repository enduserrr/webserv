/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:50:04 by asalo            ###   ########.fr       */
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

bool    ServerLoop::hasTimedOut() {
    time_t currentTime = time(nullptr);
    return (currentTime - _startUpTime) >= 300;
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
            logStream << "Server started on port(s): " << *portIt;
            Logger::getInstance().logLevel("INFO", logStream.str(), 0);
        }
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
    if (serverFull())
        return ;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientFd < 0) {
        Logger::getInstance().logLevel("SYS_ERROR", "Failed to accept client connection.", 1);
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
    session._block = _portToBlock[localPort];
    _clients[clientFd] = session;

    struct pollfd pfd;// Add the new client to the poll vector.
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);
    std::ostringstream logStream;
    logStream << "New client connected on port: " << localPort  << " (fd: " << clientFd << ")";
    Logger::getInstance().logLevel("INFO", logStream.str(), 0);
}

/**
 * @brief   Reads and accumulates received data into a given clients buffer
 *          and determines whether received request is single or multi part.
 */
void ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[4096]; // For each recv
    ssize_t bytesRead;
    HttpParser parser;

    while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        _clients[clientSocket].buffer.append(buffer, bytesRead);
        if (parser.isFullRequest(_clients[clientSocket].buffer))
            break ;
        if (parser.getState() != 0) {
            Logger::getInstance().logLevel("SYS_ERROR", "Bad request (stoi fail).", 1);
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
        std::cout<< "PARSER return state: " << parser.getState() << std::endl;
        std::string respons =  "HTTP/1.1" + std::to_string(parser.getState()) + "\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: 52\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "<html><body><h1> request return state: " + std::to_string(parser.getState()) + "</h1></body></html>";
        sendResponse(clientSocket, respons);
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

void ServerLoop::startServer() {
    setupServerSockets();
    _startUpTime = time(nullptr);

    while (_run) {
        // if (hasTimedOut()) {
        //     std::cout << "No activity for 10 seconds. Exiting server." << std::endl;
        //     closeServer();
        //     return;
        // }
        int pollResult = poll(_pollFds.data(), _pollFds.size(), 5000);
        if (pollResult < 0) {
            Logger::getInstance().logLevel("WARNING", "Error in poll().", 0);
            break ;
        }
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents & POLLIN) {
                int fd = _pollFds[i].fd;
                if (std::find(_serverSockets.begin(), _serverSockets.end(), fd) != _serverSockets.end()) {
                    acceptNewConnection(fd);
                } else {
                    handleClientRequest(fd);
                }
            }
        }
    }
}

void ServerLoop::removeClient(int clientFd) {
    std::map<int, ClientSession>::iterator it = _clients.find(clientFd);
    if (it != _clients.end()) {
        close(clientFd);
        _clients.erase(it);
        _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
                        [clientFd](struct pollfd& pfd) { return pfd.fd == clientFd; }),
                        _pollFds.end());
        std::ostringstream logStream;
        logStream << "Client disconnected (fd: " << clientFd << ")";
        Logger::getInstance().logLevel("INFO", logStream.str(), 0);
    }
}

void ServerLoop::closeServer() {
    for (std::vector<struct pollfd>::const_iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        close(it->fd);
    }
    _pollFds.clear();
    Logger::getInstance().logLevel("INFO", "Server closed and resources cleaned.", 0);
}

