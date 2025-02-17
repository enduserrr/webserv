/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 11:30:12 by asalo             #+#    #+#             */
/*   Updated: 2025/02/17 11:36:24 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Webserver.hpp"
#include "Libs.hpp"
#include <csignal>
#include <iostream>

ServerLoop *g_serverLoop = nullptr;

void signalHandler(int signum) {
    std::cout << "\nSignal (" << signum << ") received, stopping server..." << std::endl;
    if (g_serverLoop)
        g_serverLoop->stop();
}

int main(int ac, char **av) {
    if (ac != 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        return 1;
    }
    std::cout << "\nOpening program...\n" << std::endl;

    ConfParser configParse(av[1]);
    try {
        configParse.fileValidation();
        configParse.parseFile();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "\nShutting down program..." << std::endl;
        return 1;
    }
    std::vector<ServerBlock> serverBlocks = configParse.getServers();
    ServerLoop serverLoop(serverBlocks);
    g_serverLoop = &serverLoop;

    // Register signal handlers for graceful shutdown.
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    serverLoop.startServer();
    return 0;
}

/* int main(int ac, char **av) {
    if (ac != 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        return 1;
    }
    std::cout << "\nOpening program...\n" << std::endl;

    ConfParser configParse(av[1]);
    try {
        configParse.fileValidation();
        configParse.parseFile();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "\nShutting down program..." << std::endl;
        return 1;
    }
    std::vector<ServerBlock> serverBlocks = configParse.getServers();
    ServerLoop serverLoop(serverBlocks);
    g_serverLoop = &serverLoop;

    // Register signal handlers for graceful shutdown.
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Run the server loop in a separate thread.
    std::thread serverThread([&serverLoop]() {
        serverLoop.startServer();
    });

    // Give the server a moment to start.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // --- Simulate a client connection to 127.0.0.1:8080 ---
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock < 0) {
        std::cerr << "Failed to create client socket" << std::endl;
        return 1;
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return 1;
    }

    if (connect(clientSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }

    // Send a simple GET request (no Content-Length header)
    std::string request = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\n\r\n";
    if (send(clientSock, request.c_str(), request.size(), 0) < 0) {
        std::cerr << "Failed to send request" << std::endl;
    } else {
        std::cout << "Client sent request:\n" << request << std::endl;
    }

    // Read and display the response.
    char respBuffer[1024];
    ssize_t respBytes = recv(clientSock, respBuffer, sizeof(respBuffer) - 1, 0);
    if (respBytes > 0) {
        respBuffer[respBytes] = '\0';
        std::cout << "Client received response:\n" << respBuffer << std::endl;
    } else {
        std::cout << "Client received no response" << std::endl;
    }
    close(clientSock);
    // --- End simulation ---

    // Stop the server and wait for the server thread to finish.
    serverLoop.stop();
    serverThread.join();

    return 0;
} */
