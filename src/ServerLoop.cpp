/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/02/12 12:13:45 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/ServerLoop.hpp"
#include "../incs/StaticHandler.hpp"
#include "../incs/Router.hpp"
#include "../incs/ServerBlock.hpp"
#include <algorithm>
#include <iostream>
#include <cstring> //memset
#include <unistd.h> //close()
#include <arpa/inet.h> //socket operations

ServerLoop::ServerLoop() {}

ServerLoop::ServerLoop(const std::vector<ServerBlock> &serverBlocks)
    : _serverBlocks(serverBlocks) {}

ServerLoop::~ServerLoop() {
    closeServer();
}

bool    ServerLoop::hasTimedOut() {
    time_t currentTime = time(nullptr);
    return (currentTime - _startUpTime) >= 300;  // Timeout after 5 minutes
}

void ServerLoop::setupServerSockets() {
    for (std::vector<ServerBlock>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
        const std::vector<int>& ports = it->getPorts();  // Get the vector of ports

        for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket < 0) {
                ErrorHandler::getInstance().logError("Failed to create socket for port " + std::to_string(*portIt));
                continue;
            }

            int opt = 1;  // Set socket options
            if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                ErrorHandler::getInstance().logError("Failed to set socket options for port " + std::to_string(*portIt));
                close(serverSocket);
                continue;
            }

            struct sockaddr_in serverAddr;  // Bind socket
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            // Bind to 127.0.0.1, should be changed later once parsing is updated
            if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
                ErrorHandler::getInstance().logError("Invalid address for binding to port " + std::to_string(*portIt));
                close(serverSocket);
                continue;
            }
            serverAddr.sin_port = htons(*portIt);  // Convert int to network byte order

            if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                ErrorHandler::getInstance().logError("Failed to bind socket for port " + std::to_string(*portIt));
                close(serverSocket);
                continue;
            }

            if (listen(serverSocket, SOMAXCONN) < 0) {
                ErrorHandler::getInstance().logError("Failed to listen on port " + std::to_string(*portIt));
                close(serverSocket);
                continue;
            }

            struct pollfd pfd;  // Add to pollfd
            pfd.fd = serverSocket;
            pfd.events = POLLIN;
            _pollFds.push_back(pfd);

            std::cout << "Server started on port: " << *portIt << std::endl;
        }
    }
}

void    ServerLoop::acceptNewConnection(int serverSocket) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        std::cerr << RB "Error:" RES " accepting connection" << std::endl;
        return ;
    }
    struct pollfd pfd;// Add client socket to pollfd vector
    pfd.fd = clientSocket;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);
    std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

void ServerLoop::handleClientRequest(int clientSocket) {
    std::cout << "REQUEST RECEIVED!" << std::endl;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    // Client disconnected
    if (bytesRead == 0) {
        std::cout << "Client closed the connection." << std::endl;
        close(clientSocket);
        _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
            [clientSocket](const struct pollfd &pfd) { return pfd.fd == clientSocket; }),
            _pollFds.end());
        return ;
    } else if (bytesRead < 0) { // Error occurred
        ErrorHandler::getInstance().logError("Error receiving data from client.");
        close(clientSocket);
        _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
            [clientSocket](const struct pollfd &pfd) { return pfd.fd == clientSocket; }),
            _pollFds.end());
        return ;
    }

    std::string request(buffer, bytesRead);
    std::cout << "Received request: " << request << std::endl;

    // Select the relevant ServerBlock (for simplicity, we use the first one)
    ServerBlock &block = _serverBlocks[0];

    HttpParser parser;
    if (!parser.parseRequest(block, request, block.getBodySize())) {
        std::string errorResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(400);
        sendResponse(clientSocket, errorResponse);
        return ;
    }

    // Use the ServerBlock to complete the creation of the HttpRequest
    if (!parser.createRequest(block)) { // createRequest applies autoIndex and root settings
        std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(500);
        sendResponse(clientSocket, errorResponse);
        return ;
    }

    HttpRequest req = parser.getPendingRequest();
    // std::cout << RB << "ROOT: " << req.getRoot() << "\nAUTO-INDEX: " << req.getAutoIndex() << RES << std::endl;
    std::string response = Router().routeRequest(req);
    sendResponse(clientSocket, response);
    parser.removeRequest();
}

void    ServerLoop::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        ErrorHandler::getInstance().logError("Failed to send response to client.");
        std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(500);
        send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
    }
}


void    ServerLoop::startServer() {
    setupServerSockets();

    _startUpTime = time(nullptr); // Set start up time
    while (true) {
        if (hasTimedOut()) {// Check for timeout (exit after 10 sec)
            std::cout << "No activity for 10 seconds. Exiting server." << std::endl;
            closeServer();
            return ;
        }
        int pollResult = poll(_pollFds.data(), _pollFds.size(), 5000);
        if (pollResult < 0) {
            std::cerr << RB "Error:" RES " in poll()" << std::endl;
            break ;
        }
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents & POLLIN) {
                if (i < _serverBlocks.size()) {
                    acceptNewConnection(_pollFds[i].fd);
                } else {
                    handleClientRequest(_pollFds[i].fd);
                }
            }
        }
    }
}

void ServerLoop::closeServer() {
    for (std::vector<struct pollfd>::const_iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        close(it->fd);
    }

    _pollFds.clear();
    _clientData.clear();
    std::cout << "Server closed and resources cleaned." << std::endl;
}
