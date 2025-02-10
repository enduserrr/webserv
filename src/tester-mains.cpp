/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tester-mains.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 11:30:12 by asalo             #+#    #+#             */
/*   Updated: 2025/02/10 09:07:40 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Webserver.hpp"
#include "Libs.hpp"

/**
 * @brief Test: something.
 */
/* int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        return 1;
    }
    std::cout << "\nOpening program..\n\n" << std::endl;
    ConfParser configParse(argv[1]);
    if (!configParse.fileValidation()) {
        std::cout << "File validation failed, shutting down.." << std::endl;
        return 1;
    }
    if (!configParse.parseFile()) {
        std::cout << "File parsing failed, shutting down.." << std::endl;
        return 1;
    }
    if (DEBUG == 1)
        configParse.display();

    std::vector<ServerBlock> serverBlocks = configParse.getServers();
    // TRANSFER CUSTOM ERROR PAGES FROM CONF PARSER TO ERRORHANDLER
    // ErrorHandler::getInstance().logError("Loading custom error pages...");
    // for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
    //     it->setErrorPages(configParse._errorPages);
    // }

    ServerLoop serverLoop(serverBlocks);
    serverLoop.startServer();

    return 0;
} */



/**
 * @brief   Tests: Everything.
 */
/* int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        return 1;
    }
    const char* configFile = argv[1];

    std::thread serverThread([configFile]() {
        std::cout << "\n[Server] Opening program...\n" << std::endl;
        ConfParser configParse(configFile);
        if (!configParse.fileValidation()) {
            std::cerr << "File validation failed, shutting down." << std::endl;
            return;
        }
        if (!configParse.parseFile()) {
            std::cerr << "File parsing failed, shutting down." << std::endl;
            return;
        }
        std::vector<ServerBlock> serverBlocks = configParse.getServers();
        ErrorHandler::getInstance().logError("Adding custom error pages...");
        for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
            it->setErrorPages(configParse._errorPages);
        }
        ServerLoop serverLoop(serverBlocks);
        serverLoop.startServer();
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto simulateClientRequest = [](const std::string &requestDescription, const std::string &requestData) {
        std::cout << "\n[Client] " << requestDescription << std::endl;
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket < 0) {
            std::cerr << "Client socket creation failed." << std::endl;
            return;
        }
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080);
        if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
            std::cerr << "Invalid address." << std::endl;
            close(clientSocket);
            return;
        }
        if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Connection to server failed." << std::endl;
            close(clientSocket);
            return;
        }
        if (send(clientSocket, requestData.c_str(), requestData.size(), 0) < 0) {
            std::cerr << "Send failed." << std::endl;
            close(clientSocket);
            return;
        }
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived < 0) {
            std::cerr << "Receive failed." << std::endl;
        } else {
            std::cout << "\n[Client] Received response:\n" << buffer << std::endl;
        }
        close(clientSocket);
    };

    // // Simulate a static resource request (for /index.html)
    // std::string staticRequest = "GET /index.html HTTP/1.1\r\n"
    //                             "Host: localhost\r\n"
    //                             "\r\n";
    // simulateClientRequest("Requesting static resource (/www/index.html)", staticRequest);

    // // Simulate a CGI request (for /cgi-bin/test.php)
    // std::string cgiRequest = "GET /cgi-bin/test.php HTTP/1.1\r\n"
    //                          "Host: localhost\r\n"
    //                          "\r\n";
    // simulateClientRequest("Requesting CGI resource (/cgi-bin/test.php)", cgiRequest);

    // // Sleep and join the server thread.
    // std::this_thread::sleep_for(std::chrono::seconds(2));
    // serverThread.join();
    // return 0;

    // Simulate a static GET request for /index.html.
    std::string staticGetRequest = "GET /index.html HTTP/1.1\r\n"
                                   "Host: localhost\r\n"
                                   "\r\n";
    simulateClientRequest("Requesting static resource (/index.html)", staticGetRequest);

    // Simulate a CGI GET request for /cgi-bin/test.php.
    std::string cgiGetRequest = "GET /cgi-bin/test.php HTTP/1.1\r\n"
                                "Host: localhost\r\n"
                                "\r\n";
    simulateClientRequest("Requesting CGI resource (/cgi-bin/test.php)", cgiGetRequest);

    // Simulate a static POST request (file upload) for /index.html.
    // Here, we assume that a POST to /index.html is routed to StaticHandler, which will invoke UploadHandler.
    std::string staticPostRequest = "POST /index.html HTTP/1.1\r\n"
                                    "Host: localhost\r\n"
                                    "Content-Length: 27\r\n"
                                    "\r\n"
                                    "This is a test static upload";
    simulateClientRequest("Static POST request (upload to /index.html)", staticPostRequest);

    // Simulate a CGI POST request for /cgi-bin/test.php.
    std::string cgiPostRequest = "POST /cgi-bin/test.php HTTP/1.1\r\n"
                                 "Host: localhost\r\n"
                                 "Content-Length: 25\r\n"
                                 "\r\n"
                                 "This is a test CGI upload";
    simulateClientRequest("CGI POST request", cgiPostRequest);
    // Sleep and join the server thread.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    serverThread.join();
    return 0;
} */
