/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 11:30:12 by asalo             #+#    #+#             */
/*   Updated: 2025/02/10 09:49:13 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Webserver.hpp"
#include "Libs.hpp"

// int main(int ac, char **av) {
//     if (ac != 2) {
//         std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
//         return 1;
//     }
//     const char* configFile = av[1];

//     // Start the server in a separate thread (simulate client/server interaction)
//     std::thread serverThread([configFile]() {
//         std::cout << "\n[Server] Opening program...\n" << std::endl;
//         ConfParser configParse(configFile);
//         if (!configParse.fileValidation()) {
//             std::cerr << "File validation failed, shutting down." << std::endl;
//             return;
//         }
//         if (!configParse.parseFile()) {
//             std::cerr << "File parsing failed, shutting down." << std::endl;
//             return;
//         }
//         std::vector<ServerBlock> serverBlocks = configParse.getServers();
//         ErrorHandler::getInstance().logError("Adding custom error pages...");
//         for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
//             it->setErrorPages(configParse._errorPages);
//         }
//         ServerLoop serverLoop(serverBlocks);
//         serverLoop.startServer();
//     });

//     // Sleep to ensure the server is up
//     std::this_thread::sleep_for(std::chrono::seconds(3));

//     /**
//      * @brief   A lambda is an anonymous function object that hold short function without having to
//      *          declare whole separate function for it.
//      *
//      *          Below the lambda is simulating a client request without having to define
//      *          a separate named function for it.
//      */
//     std::function<void(const std::string&, const std::string&)> simulateClientRequest =
//     [](const std::string &requestDescription, const std::string &requestData) {
//         std::cout << "\n[Client] " << requestDescription << std::endl;
//         int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//         if (clientSocket < 0) {
//             std::cerr << "Client socket creation failed." << std::endl;
//             return;
//         }
//         struct sockaddr_in serverAddr;
//         serverAddr.sin_family = AF_INET;
//         serverAddr.sin_port = htons(8080);
//         if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
//             std::cerr << "Invalid address." << std::endl;
//             close(clientSocket);
//             return;
//         }
//         if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
//             std::cerr << "Connection to server failed." << std::endl;
//             close(clientSocket);
//             return;
//         }
//         if (send(clientSocket, requestData.c_str(), requestData.size(), 0) < 0) {
//             std::cerr << "Send failed." << std::endl;
//             close(clientSocket);
//             return;
//         }
//         char buffer[4096];
//         memset(buffer, 0, sizeof(buffer));
//         int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
//         if (bytesReceived < 0) {
//             std::cerr << "Receive failed." << std::endl;
//         } else {
//             std::cout << "\n[Client] Received response:\n" << buffer << std::endl;
//         }
//         close(clientSocket);
//     };

// /*     // Static 'GET' request for /index.html
//     std::string staticGetRequest = "GET /index.html HTTP/1.1\r\n"
//                                    "Host: localhost\r\n"
//                                    "\r\n";
//     simulateClientRequest("Requesting static resource (/index.html)", staticGetRequest);

//     // CGI 'GET' request for /cgi-bin/test.php
//     std::string cgiGetRequest = "GET /cgi-bin/test.php HTTP/1.1\r\n"
//                                 "Host: localhost\r\n"
//                                 "\r\n";
//     simulateClientRequest("Requesting CGI resource (/cgi-bin/test.php)", cgiGetRequest);

//     // Static 'POST' request (file upload) for /index.html
//     std::string staticPostRequest = "POST /index.html HTTP/1.1\r\n"
//                                     "Host: localhost\r\n"
//                                     "Content-Length: 27\r\n"
//                                     "\r\n"
//                                     "This is a test static upload";
//     simulateClientRequest("Static POST request (upload to /index.html)", staticPostRequest);

//     // CGI 'POST' request for /cgi-bin/test.php
//     std::string cgiPostRequest = "POST /cgi-bin/test.php HTTP/1.1\r\n"
//                                  "Host: localhost\r\n"
//                                  "Content-Length: 25\r\n"
//                                  "\r\n"
//                                  "This is a test CGI upload";
//     simulateClientRequest("CGI POST request", cgiPostRequest); */

//     // 'GET' request for the /uploads/ (directory)
//     std::string directoryRequest = "GET /uploads/ HTTP/1.1\r\n"
//                                    "Host: localhost\r\n"
//                                    "\r\n";
//     simulateClientRequest("Requesting directory listing for /uploads/", directoryRequest);

//     // Sleep and join threads (why sleep first?)
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     serverThread.join();
//     return 0;
// }

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        return 1;
    }
    std::cout << "\nOpening program..\n\n" << std::endl;
    ConfParser configParse(argv[1]);
    try {
        configParse.fileValidation();
        configParse.parseFile();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "\nshutting down program.." << std::endl; 
        return 1;
    }
    if (DEBUG == 1)
        configParse.display();

    // std::vector<ServerBlock> serverBlocks = configParse.getServers();
    // // TRANSFER CUSTOM ERROR PAGES FROM CONF PARSER TO ERRORHANDLER
    // ErrorHandler::getInstance().logError("Loading custom error pages...");
    // for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
    //     it->setErrorPages(configParse._errorPages);
    // }

    // ServerLoop serverLoop(serverBlocks);
    // serverLoop.startServer();

    return 0;
}