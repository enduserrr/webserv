/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/02/19 12:13:30 by asalo            ###   ########.fr       */
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
    return (currentTime - _startUpTime) >= 300;  // Timeout after 5 minutes
}

void ServerLoop::setupServerSockets() {
    for (std::vector<ServerBlock>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
        const std::vector<int>& ports = it->getPorts();

        for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
            if (std::find(_boundPorts.begin(), _boundPorts.end(), *portIt) != _boundPorts.end()) {
                continue; // Skip duplicate port binding
            }

            int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket < 0) {
                ErrorHandler::getInstance().logError("Failed to create socket for port " + std::to_string(*portIt));
                continue;
            }

            int opt = 1;
            if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
                ErrorHandler::getInstance().logError("Failed to set socket options for port " + std::to_string(*portIt));
                close(serverSocket);
                continue;
            }
            struct sockaddr_in serverAddr;
            memset(&serverAddr, 0, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(*portIt);
            if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
                ErrorHandler::getInstance().logError("Invalid address for port " + std::to_string(*portIt));
                close(serverSocket);
                continue;
            }
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
            struct pollfd pfd;
            pfd.fd = serverSocket;
            pfd.events = POLLIN;
            _pollFds.push_back(pfd);
            _serverSockets.push_back(serverSocket);
            _boundPorts.push_back(*portIt);
            _portToBlock[*portIt] = *it;  // Associate port with first ServerBlock
            std::cout << "Server started on port: " << *portIt << std::endl;
        }
    }
}

void ServerLoop::acceptNewConnection(int serverSocket) {
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientFd = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientFd < 0) {
        ErrorHandler::getInstance().logError("Failed to accept client connection.");
        return ;
    }

    struct sockaddr_in localAddr; // Retrieve the local (server) port for the accepted connection.
    socklen_t localLen = sizeof(localAddr);
    if (getsockname(clientFd, (struct sockaddr*)&localAddr, &localLen) < 0) {
        ErrorHandler::getInstance().logError("Failed to get local address for client connection.");
        close(clientFd);
        return ;
    }
    int localPort = ntohs(localAddr.sin_port);
    if (_portToBlock.find(localPort) == _portToBlock.end()) {// Find ServerBlock for this port
        std::cerr << "Warning: No ServerBlock found for port " << localPort << std::endl;
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
    std::cout << "New client connected on port " << localPort
              << " (fd: " << clientFd << ")" << std::endl;
    // _clientCount++;
}

void ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead == 0) { // Client closed connection
        std::cout << "Client disconnected: " << clientSocket << std::endl;
        close(clientSocket);
        _clients.erase(clientSocket);
        return ;
    } else if (bytesRead < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return ; // No data available yet, try again later
        } else if (errno == ENOTCONN) {
            std::cerr << "Client already disconnected: " << clientSocket << std::endl;
        } else {
            std::cerr << "Error reading from socket " << clientSocket << ": " << strerror(errno) << std::endl;
        }
        close(clientSocket);
        std::cout << "Removing socket: " << clientSocket << std::endl;
        _clients.erase(clientSocket);
        return ;
    }

    std::string data(buffer, bytesRead); // Store received data
    if (_clients.find(clientSocket) == _clients.end()) {// Ensure client session exists
        _clients[clientSocket] = ClientSession(clientSocket);
        struct sockaddr_in addr;// Retrieve the port the client is connected to
        socklen_t addrLen = sizeof(addr);
        if (getsockname(clientSocket, (struct sockaddr*)&addr, &addrLen) == 0) {
            int port = ntohs(addr.sin_port);
            if (_portToBlock.find(port) != _portToBlock.end()) { // Assign the correct ServerBlock based on the port
                _clients[clientSocket]._block = _portToBlock[port];
            } else {
                std::cerr << "Warning: No matching ServerBlock for port " << port << std::endl;
            }
        }
    }

    ClientSession &client = _clients[clientSocket];
    
    //ClientSession monitoroi ettei requesteja tuu liikaa. nyt setattu 10/sekunti. 
    if (client.requestLimiter()) {
        std::string response = "HTTP/1.1 429 Too Many Requests\r\n"
                               "Content-Type: text/html\r\n\r\n"
                               "<h1>429 Too Many Requests</h1>";
        sendResponse(clientSocket, response);
        return;
    }


    client.buffer += data;
    ServerBlock &block = client._block; // Correct ServerBlock to the client
    HttpParser parser;
    std::istringstream input(client.buffer);
    if (parser.readFullRequest(input, block)) {
        client.request = parser.getPendingRequest();
        std::string response = Router().routeRequest(client.request, clientSocket);
        sendResponse(clientSocket, response);
        client.buffer.clear();
    }
}

void    ServerLoop::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        ErrorHandler::getInstance().logError("Failed to send response to client.");
        std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(500);
        send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
    }
}

void ServerLoop::startServer() {
    setupServerSockets();
    _startUpTime = time(nullptr); // Set start up time
    // _clientCount = 0;

    while (_run) {
        if (hasTimedOut()) {
            std::cout << "No activity for 10 seconds. Exiting server." << std::endl;
            closeServer();
            return;
        }
        int pollResult = poll(_pollFds.data(), _pollFds.size(), 5000);
        if (pollResult < 0) {
            std::cerr << RB "Error:" RES " in poll()" << std::endl;
            break;
        }
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            if (_pollFds[i].revents & POLLIN) {
                int fd = _pollFds[i].fd;
                if (std::find(_serverSockets.begin(), _serverSockets.end(), fd) != _serverSockets.end()) {
                    acceptNewConnection(fd); // New client
                } else {
                    handleClientRequest(fd); // Existing client request
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
        std::cout << "Client disconnected (fd: " << clientFd << ")" << std::endl;
    }
}

void ServerLoop::closeServer() {
    for (std::vector<struct pollfd>::const_iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        close(it->fd);
    }
    _pollFds.clear();
    std::cout << "Server closed and resources cleaned." << std::endl;
}

/* void ServerLoop::test(){
    std::string chunk =
    "POST /upload HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: 16\r\n"
    "\r\n";

    HttpParser par;
    par.parseRequest(_serverBlocks[0], chunk, 100);
    HttpRequest req;
    req = par.getPendingRequest();

    std::cout<< req.getBodySize() << std::endl;
    std::cout<< req.getMethod() << std::endl;
    std::cout<< req.getUri() << std::endl;
    std::cout<< req.getHttpVersion() << std::endl;
    // std::cout<< req.getUriQuery() << std::endl;
    // std::cout<< req.getHeaders() << std::endl;
    std::cout<< req.getBody() << std::endl;
} */

