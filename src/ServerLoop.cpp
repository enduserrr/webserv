/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:19:46 by asalo             #+#    #+#             */
/*   Updated: 2025/02/15 13:10:50 by asalo            ###   ########.fr       */
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

// void ServerLoop::setupServerSockets() {
//     for (std::vector<ServerBlock>::iterator it = _serverBlocks.begin(); it != _serverBlocks.end(); ++it) {
//         const std::vector<int>& ports = it->getPorts();  // Get the vector of ports

//         for (std::vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt) {
//             int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
//             if (serverSocket < 0) {
//                 ErrorHandler::getInstance().logError("Failed to create socket for port " + std::to_string(*portIt));
//                 continue;
//             }

//             int opt = 1;  // Set socket options
//             if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
//                 ErrorHandler::getInstance().logError("Failed to set socket options for port " + std::to_string(*portIt));
//                 close(serverSocket);
//                 continue;
//             }

//             struct sockaddr_in serverAddr;  // Bind socket
//             memset(&serverAddr, 0, sizeof(serverAddr));
//             serverAddr.sin_family = AF_INET;
//             // Bind to 127.0.0.1, should be changed later once parsing is updated
//             if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
//                 ErrorHandler::getInstance().logError("Invalid address for binding to port " + std::to_string(*portIt));
//                 close(serverSocket);
//                 continue;
//             }
//             serverAddr.sin_port = htons(*portIt);  // Convert int to network byte order

//             if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
//                 ErrorHandler::getInstance().logError("Failed to bind socket for port " + std::to_string(*portIt));
//                 close(serverSocket);
//                 continue;
//             }

//             if (listen(serverSocket, SOMAXCONN) < 0) {
//                 ErrorHandler::getInstance().logError("Failed to listen on port " + std::to_string(*portIt));
//                 close(serverSocket);
//                 continue;
//             }

//             struct pollfd pfd;  // Add to pollfd
//             pfd.fd = serverSocket;
//             pfd.events = POLLIN;
//             _pollFds.push_back(pfd);

//             std::cout << "Server started on port: " << *portIt << std::endl;
//         }
//     }
// }

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
        return;
    }

    int port = 0;
    for (std::map<int, ServerBlock>::iterator it = _portToBlock.begin(); it != _portToBlock.end(); ++it) {
        if (it->first == ntohs(clientAddr.sin_port)) {
            port = it->first;
            break;
        }
    }

    if (port == 0) {
        close(clientFd);
        return;
    }

    ClientSession session(clientFd);
    session._block = _portToBlock[port]; // Assign correct ServerBlock

    _clients[clientFd] = session;

    struct pollfd pfd;
    pfd.fd = clientFd;
    pfd.events = POLLIN;
    _pollFds.push_back(pfd);

    std::cout << "New client connected on port " << port << " (fd: " << clientFd << ")" << std::endl;
}


// void    ServerLoop::acceptNewConnection(int serverSocket) {
//     struct sockaddr_in clientAddr;
//     socklen_t clientAddrLen = sizeof(clientAddr);
//     int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
//     if (clientSocket < 0) {
//         std::cerr << RB "Error:" RES " accepting connection" << std::endl;
//         return ;
//     }
//     struct pollfd pfd;// Add client socket to pollfd vector
//     pfd.fd = clientSocket;
//     pfd.events = POLLIN;
//     _pollFds.push_back(pfd);
//     std::cout << "New client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
// }

void ServerLoop::handleClientRequest(int clientSocket) {
    char buffer[1024];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesRead == 0) {
        // Client closed connection
        std::cout << "Client disconnected: " << clientSocket << std::endl;
        close(clientSocket);
        _clients.erase(clientSocket);
        return;
    } else if (bytesRead < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return; // No data available yet, try again later
        } else if (errno == ENOTCONN) {
            std::cerr << "Client already disconnected: " << clientSocket << std::endl;
        } else {
            std::cerr << "Error reading from socket " << clientSocket << ": " << strerror(errno) << std::endl;
        }
        close(clientSocket);
        std::cout << "Removing socket: " << clientSocket << std::endl;
        _clients.erase(clientSocket);
        return;
    }

    // Store received data
    std::string data(buffer, bytesRead);

    // Ensure client session exists
    if (_clients.find(clientSocket) == _clients.end()) {
        _clients[clientSocket] = ClientSession(clientSocket);

        // Retrieve the port the client is connected to
        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        if (getsockname(clientSocket, (struct sockaddr*)&addr, &addrLen) == 0) {
            int port = ntohs(addr.sin_port);

            // Assign the correct ServerBlock based on the port
            if (_portToBlock.find(port) != _portToBlock.end()) {
                _clients[clientSocket]._block = _portToBlock[port];
            } else {
                std::cerr << "Warning: No matching ServerBlock for port " << port << std::endl;
            }
        }
    }
    ClientSession &client = _clients[clientSocket];

    client.buffer += data;

    // Select the correct ServerBlock assigned to this client
    ServerBlock &block = client._block;

    HttpParser parser;
    std::istringstream input(client.buffer);

    if (parser.readFullRequest(input, block)) {
        client.request = parser.getPendingRequest();
        std::string response = Router().routeRequest(client.request, clientSocket);
        sendResponse(clientSocket, response);
        client.buffer.clear();
    }
}


// void ServerLoop::handleClientRequest(int clientSocket) {
//     char buffer[1024];
//     ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

//     if (bytesRead == 0) {
//         // Client closed connection
//         std::cout << "Client disconnected: " << clientSocket << std::endl;
//         close(clientSocket);
//         _clients.erase(clientSocket);
//         return;
//     } else if (bytesRead < 0) {
//         if (errno == EAGAIN || errno == EWOULDBLOCK) {
//             return; // No data available yet, try again later
//         } else if (errno == ENOTCONN) {
//             std::cerr << "Client already disconnected: " << clientSocket << std::endl;
//         } else {
//             std::cerr << "Error reading from socket " << clientSocket << ": " << strerror(errno) << std::endl;
//         }
//         close(clientSocket);
//         std::cout << "Removing socket: " << clientSocket << std::endl;
//         _clients.erase(clientSocket);
//         return;
//     }

//     // Store received data
//     std::string data(buffer, bytesRead);

//     // Ensure client session exists
//     if (_clients.find(clientSocket) == _clients.end()) {
//         _clients[clientSocket] = ClientSession(clientSocket);
//     }
//     ClientSession &client = _clients[clientSocket];

//     // client.buffer += data;

//     // Select the relevant ServerBlock (for simplicity, using the first one)
//     ServerBlock &block = _serverBlocks[0];

//     HttpParser parser;
//     std::istringstream input(client.buffer);

//     if (parser.readFullRequest(input, block)) {
//         client.request = parser.getPendingRequest();
//         std::string response = Router().routeRequest(client.request, clientSocket);
//         sendResponse(clientSocket, response);
//         client.buffer.clear();
//     }
// }



/*// void ServerLoop::handleClientRequest(int clientSocket) {
//     std::cout << "REQUEST RECEIVED!" << std::endl;
//     HttpParser parser;
//     std::string fullRequestData;

//     while (true) {
//         char buffer[1024];
//         memset(buffer, 0, sizeof(buffer));
//         ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

//         if (bytesRead == 0) { // Client disconnected
//             std::cout << "Client closed the connection." << std::endl;
//             close(clientSocket);
//             _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
//                 [clientSocket](const struct pollfd &pfd) { return pfd.fd == clientSocket; }),
//                 _pollFds.end());
//             return;
//         } else if (bytesRead < 0) { // Error occurred
//             ErrorHandler::getInstance().logError("Error receiving data from client.");
//             close(clientSocket);
//             _pollFds.erase(std::remove_if(_pollFds.begin(), _pollFds.end(),
//                 [clientSocket](const struct pollfd &pfd) { return pfd.fd == clientSocket; }),
//                 _pollFds.end());
//             return;
//         }

//         fullRequestData.append(buffer, bytesRead);
//         std::istringstream iss(fullRequestData);

//         // Select the relevant ServerBlock (for simplicity, we use the first one)
//         ServerBlock &block = _serverBlocks[0];
//         if (parser.readFullRequest(iss, block))
//             break;
//         // Otherwise, continue accumulating data.
//     }

//     HttpRequest req = parser.getPendingRequest();
//     // std::cout << RB << "ROOT: " << req.getRoot() << "\nAUTO-INDEX: " << req.getAutoIndex() << RES << std::endl;
//     std::string response = Router().routeRequest(req);
//     sendResponse(clientSocket, response);
//     parser.removeRequest();
// }*/


void    ServerLoop::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        ErrorHandler::getInstance().logError("Failed to send response to client.");
        std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(500);
        send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
    }
}


// void    ServerLoop::startServer() {
//     setupServerSockets();

//     _startUpTime = time(nullptr); // Set start up time
//     while (_run) {
//         if (hasTimedOut()) {// Check for timeout (exit after 10 sec)
//             std::cout << "No activity for 10 seconds. Exiting server." << std::endl;
//             closeServer();
//             return ;
//         }
//         int pollResult = poll(_pollFds.data(), _pollFds.size(), 5000);
//         if (pollResult < 0) {
//             std::cerr << RB "Error:" RES " in poll()" << std::endl;
//             break ;
//         }
//         for (size_t i = 0; i < _pollFds.size(); ++i) {
//             if (_pollFds[i].revents & POLLIN) {
//                 if (i < _serverBlocks.size()) {
//                     acceptNewConnection(_pollFds[i].fd);
//                 } else {
//                     handleClientRequest(_pollFds[i].fd);
//                 }
//             }
//         }
//     }
// }

void ServerLoop::startServer() {
    setupServerSockets();
    _startUpTime = time(nullptr); // Set start up time

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
                    acceptNewConnection(fd); // Handle new client
                } else {
                    handleClientRequest(fd); // Handle existing client request
                }
            }
        }
    }
}

void ServerLoop::closeConnection(int clientFd) {
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
    _clientData.clear();
    std::cout << "Server closed and resources cleaned." << std::endl;
}


// void ServerLoop::test(){
//     std::string chunk =
//     "POST /upload HTTP/1.1\r\n"
//     "Host: example.com\r\n"
//     "Content-Type: application/x-www-form-urlencoded\r\n"
//     "Content-Length: 16\r\n"
//     "\r\n";

//     HttpParser par;
//     par.parseRequest(_serverBlocks[0], chunk, 100);
//     HttpRequest req;
//     req = par.getPendingRequest();

//     std::cout<< req.getBodySize() << std::endl;
//     std::cout<< req.getMethod() << std::endl;
//     std::cout<< req.getUri() << std::endl;
//     std::cout<< req.getHttpVersion() << std::endl;
//     // std::cout<< req.getUriQuery() << std::endl;
//     // std::cout<< req.getHeaders() << std::endl;
//     std::cout<< req.getBody() << std::endl;


// }

void ServerLoop::removeClient(int clientSocket) {
    if (_clients.find(clientSocket) != _clients.end()) {
        _clients[clientSocket].closeConnection();
        _clients.erase(clientSocket);
    }
}

