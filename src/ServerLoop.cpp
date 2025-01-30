/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/01/26 16:19:51 by asalo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ServerLoop.hpp"
#include <algorithm>
#include <iostream>
#include <cstring> //memset
#include <unistd.h> //close()
#include <arpa/inet.h> //socket operations

ServerLoop::ServerLoop() {}

ServerLoop::ServerLoop(const std::vector<ServerBlock>& serverBlocks)
    : _serverBlocks(serverBlocks) {}

ServerLoop::~ServerLoop() {
    closeServer();
}

bool    ServerLoop::hasTimedOut() {
    time_t currentTime = time(nullptr);
    return (currentTime - _startUpTime) >= 10;  // 10-second timeout
}

void    ServerLoop::setupServerSockets() {
    for (const auto &server : _serverBlocks) {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            ErrorHandler::getInstance().logError("Failed to create socket for port " + server.getPort());
            continue ;
        }
        int opt = 1;// Set socket options below
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            ErrorHandler::getInstance().logError("Failed to set socket options for port " + server.getPort());
            close(serverSocket);
            continue ;
        }
        struct sockaddr_in serverAddr;// Bind socket
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(std::stoi(server.getPort()));
        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            ErrorHandler::getInstance().logError("Failed to bind socket for port " + server.getPort() + ErrorHandler::getInstance().getErrorPage(404));
            close(serverSocket);
            continue ;
        }
        if (listen(serverSocket, SOMAXCONN) < 0) {
            ErrorHandler::getInstance().logError("Failed to listen on port " + server.getPort());
            close(serverSocket);
            continue ;
        }
        struct pollfd pfd;// Add to pollfd
        pfd.fd = serverSocket;
        pfd.events = POLLIN;
        _pollFds.push_back(pfd);
        std::cout << "Server started on port: " << server.getPort() << std::endl;
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

void    ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead <= 0) {
        ErrorHandler::getInstance().logError("Client disconnected or error receiving data.");
        close(clientSocket);
        _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
            [clientSocket](const struct pollfd &pfd) { return pfd.fd == clientSocket; }),
            _pollFds.end());
        return ;
    }
    std::string request(buffer, bytesRead);
    std::cout << "Received request: " << request << std::endl;
    // Simple validation (e.g., check if request starts with "GET")
    if (request.substr(0, 3) != "GET") {
        std::string errorResponse = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(400);
        sendResponse(clientSocket, errorResponse);
        return;
    }
    // Use the 404 error if the request isn't "GET"
    if (request.find("GET /") != 0) {
        std::string errorResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(404);
        sendResponse(clientSocket, errorResponse);
        return ;
    }
    std::string response = "HTTP/1.1 200 OK";
    sendResponse(clientSocket, response);
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
    for (const auto &pfd : _pollFds) {
        close(pfd.fd);
    }

    _pollFds.clear();
    _clientData.clear();
    std::cout << "Server closed and resources cleaned." << std::endl;
}
