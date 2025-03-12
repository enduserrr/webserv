/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/03/02 19:19:47 by asalo            ###   ########.fr       */
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
            _portToBlock[*portIt] = *it;  // Correct serverblock?
            std::cout << "Server started on port: " << *portIt << std::endl;
        }
    }
}

bool ServerLoop::serverFull() {
     if (_clients.size() >= MAX_CLIENTS) {
        std::cerr << "Server is full! Rejecting new client." << std::endl;
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
            std::cerr << "bad request (stoi fails)! " << std::endl; //should propably give 400 bad request errorpage? 
            return ;
        }
    }
    if (bytesRead == 0) {
        removeClient(clientSocket);
        return ;
    } else if (bytesRead < 0) {
        std::cerr << "Error reading from socket " << clientSocket
                  << ": " << strerror(errno) << std::endl;
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
                std::cerr << "Warning: No matching ServerBlock for port " << port << std::endl;
            }
        }
    } 
    if (_clients[clientSocket].requestLimiter()) {
        sendResponse(clientSocket, ErrorHandler::getInstance().getErrorPage(429));
        return;
    }

    // std::istringstream headerStream(client.buffer); // Create a temporary stream to parse headers
    // std::string line;
    // std::string contentType;

    // std::getline(headerStream, line); // Read the request line
    // while (std::getline(headerStream, line) && line != "\r") {// Read headers until an empty line
    //     if (line.find("Content-Type:") == 0) {
    //         contentType = line.substr(strlen("Content-Type:"));
    //         contentType.erase(0, contentType.find_first_not_of(" \t"));
    //     }
    // }
    /*  After getting multipart upload check it's size before forwarding
        If multipart request received only partly it should return to not block */
    // --- Multi part check ---
    // if (!contentType.empty() && contentType.find("multipart/form-data") == 0) {
    //     handleMultipartUpload(client);
    //     return ;
    // }
    if (parser.parseRequest(_clients[clientSocket]._block)) {
        _clients[clientSocket].request = parser.getPendingRequest();
        std::string response = Router().routeRequest(_clients[clientSocket].request, clientSocket);
        sendResponse(clientSocket, response);
        removeClient(clientSocket);
    } else { //Hardcoded response that shows only return state from parser --> fix later to show proper errorpage 
        std::cout<< "PARSER return state: " << parser.getState() << std::endl;
        std::string response =  "HTTP/1.1" + std::to_string(parser.getState()) + "\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: 52\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "<html><body><h1> request return state: " + std::to_string(parser.getState()) + "</h1></body></html>";
        sendResponse(clientSocket, response);
        removeClient(clientSocket);
    }
}

/**
 * @brief   Multipart specific boundary extraction, filename retrieval, saving on to disk
 *          and directing the final result directly to uploadHandler.
 *
 */
void ServerLoop::handleMultipartUpload(ClientSession &client) {
    UploadHandler uploadHandler; // Direct to UploadHandler for multipart processing.
    std::string uploadedFilePath = uploadHandler.uploadReturnPath(client.request);

    if (uploadedFilePath.find("HTTP/1.1") == 0) {
        sendResponse(client.fd, uploadedFilePath);
    } else {
        std::ostringstream responseStream;
        responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                       << "<html><body><h1>Upload Successful</h1>"
                       << "<p>File uploaded: " << uploadedFilePath << "</p></body></html>";
        sendResponse(client.fd, responseStream.str());
    }
    client.buffer.clear();
    removeClient(client.fd);
}

void    ServerLoop::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        ErrorHandler::getInstance().logError("Failed to send response to client.");
        std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(500);
        send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
    }
    return ;
}

void ServerLoop::startServer() {
    setupServerSockets();
    _startUpTime = time(nullptr);

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

