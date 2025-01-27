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

ServerLoop::ServerLoop() {
    std::cout << GC << "ServerLoop constructor call." << RES << std::endl;
}

ServerLoop::ServerLoop(const std::vector<ServerBlock>& serverBlocks)
    : _serverBlocks(serverBlocks) {
    std::cout << GC << "ServerLoop constructor call with params." << RES << std::endl;
}

ServerLoop::ServerLoop(const ServerLoop& other)
    : _serverBlocks(other._serverBlocks), _pollFds(other._pollFds), _clientData(other._clientData) {
    std::cout << GC << "ServerLoop copy constructor call." << RES << std::endl;
}

ServerLoop& ServerLoop::operator=(const ServerLoop& other) {
    if (this != &other) {
        _serverBlocks = other._serverBlocks;
        _pollFds = other._pollFds;
        _clientData = other._clientData;
    }
    std::cout << GC << "ServerLoop copy assignment operator call." << RES << std::endl;
    return (*this);
}

ServerLoop::~ServerLoop() {
    closeServer();
    std::cout << GC << "ServerLoop destructor call." << RES << std::endl;
}

// Check if the server has timed out (10 seconds of inactivity)
bool ServerLoop::hasTimedOut() {
    time_t currentTime = time(nullptr);
    return (currentTime - _startUpTime) >= 5;  // 10-second timeout
}

void ServerLoop::setupServerSockets() {
    for (const auto& server : _serverBlocks) {
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0) {
            std::cerr << RB "Error:" RES " creating socket for port " << server.getPort() << std::endl;
            continue ;
        }
        // Set socket options (e.g., reuse address)
        int opt = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << RB "Error:" RES " setting socket options for port " << server.getPort() << std::endl;
            close(serverSocket);
            continue ;
        }
        // Bind socket to port
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(std::stoi(server.getPort()));

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << RB "Error:" RES " binding socket for port " << server.getPort() << std::endl;
            close(serverSocket);
            continue ;
        }
        // Listen for incoming connections
        if (listen(serverSocket, SOMAXCONN) < 0) {
            std::cerr << RB "Error:" RES "listening on port " << server.getPort() << std::endl;
            close(serverSocket);
            continue ;
        }
        // Add server socket to pollfd vector
        struct pollfd pfd;
        pfd.fd = serverSocket;
        pfd.events = POLLIN;
        _pollFds.push_back(pfd);

        std::cout << "Server started on port: " << server.getPort() << std::endl;
    }
}

void ServerLoop::acceptNewConnection(int serverSocket) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        std::cerr << RB "Error:" RES " accepting connection" << std::endl;
        return ;
    }
    // Add client socket to pollfd vector
    struct pollfd pfd;
    pfd.fd = clientSocket;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
}

void ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        // Client disconnected or error occurred
        close(clientSocket);
        _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
            [clientSocket](const struct pollfd& pfd) { return pfd.fd == clientSocket; }),
            _pollFds.end());
        std::cout << "Client disconnected" << std::endl;
        return ;
    }
    // Process the request (like parse HTTP request)
    std::string request(buffer, bytesRead);
    std::cout << "Received request: " << request << std::endl;
    // Send a simple response (for now)
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    sendResponse(clientSocket, response);
}

/* void ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        // Client disconnected or error occurred
        close(clientSocket);

        // remove the client socket from _pollFds
        for (auto it = _pollFds.begin(); it != _pollFds.end(); ++it) {
            if (it->fd == clientSocket) {
                _pollFds.erase(it);
                break;
            }
        }
        std::cout << "Client disconnected." << std::endl;
        return ;
    }

    // Process the request (e.g., parse HTTP request)
    std::string request(buffer, bytesRead);
    std::cout << "Received request: " << request << std::endl;

    // Send a simple response (for now)
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    sendResponse(clientSocket, response);
} */

void ServerLoop::sendResponse(int clientSocket, const std::string& response) {
    send(clientSocket, response.c_str(), response.size(), 0);
}

void ServerLoop::startServer() {
    setupServerSockets();

    _startUpTime = time(nullptr); //Set's start up time
    while (true) {
       // Check for timeout (times out after 5 seconds)
        if (hasTimedOut()) {
            std::cout << "No activity for 10 seconds. Exiting server." << std::endl;
            closeServer();
            return ;
        }
        int pollResult = poll(_pollFds.data(), _pollFds.size(), 5000);
        if (pollResult < 0) {
            std::cerr << RB "Error:" RES " in poll()" << std::endl;
            break;
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
    for (const auto& pfd : _pollFds) {
        close(pfd.fd);
    }
    _pollFds.clear();
    _clientData.clear();
    std::cout << "Server closed and resources cleaned." << std::endl;
}
