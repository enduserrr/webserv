/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 11:30:12 by asalo             #+#    #+#             */
/*   Updated: 2025/02/03 11:38:38 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

# include "Webserver.hpp"

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
    ErrorHandler::getInstance().logError("Loading custom error pages...");
    for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
        it->setErrorPages(configParse._errorPages);
    }

    ServerLoop serverLoop(serverBlocks);
    serverLoop.startServer();

    return 0;
} */


/* int main() {
    std::string rawDataChunk =
    "POST /upload HTTP/1.1\r\n"
    "Host: example.com\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: 16\r\n"
    "\r\n"
    "name=John&age=30"
    "GET /index.html HTTP/1.1\r\n"
    "Host: example2.com\r\n";

    //Ei toimi viela kahdella requestilla, mutta pilkkoo ensimmaisesta yhden kokonaisen
    // std::string rawDataChunk2 =
    // "Content-Type: application/secondreq\r\n"
    // "\r\n";


    std::istringstream input(rawDataChunk);
    HttpParser parser;
    parser.readFullRequest(input);
    // std::istringstream input2(rawDataChunk2);
    // parser.readFullRequest(input2);

    //parser result for debuggin
    parser.display();

    return (0);
} */

// #include <thread>
// #include <chrono>
// #include <vector>
// #include <cstring> //memset
// #include <arpa/inet.h> //Socket
// #include <unistd.h> //close()


// int main(int argc, char **argv) {
//     if (argc != 2) {
//         std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
//         return 1;
//     }
//     const char* configFile = argv[1];
//     /* Start the server in a separate thread to simulate client/server interaction */
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
//         // if (DEBUG == 1)
//             // configParse.display();
//         std::vector<ServerBlock> serverBlocks = configParse.getServers();
//         ErrorHandler::getInstance().logError("Adding custom error pages...");//Make this happen only if needed
//         for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
//             it->setErrorPages(configParse._errorPages);
//         }
//         ServerLoop serverLoop(serverBlocks);
//         serverLoop.startServer();
//     });
//     /* Wait time to ensure server is up */
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     /* --- Simulate Client Request --- */
//     std::cout << "\n[Client] Connecting to server on port 8080..." << std::endl;
//     int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
//     if (clientSocket < 0) {
//         std::cerr << "Client socket creation failed." << std::endl;
//         return 1;
//     }
//     struct sockaddr_in serverAddr;
//     serverAddr.sin_family = AF_INET;
//     serverAddr.sin_port = htons(8080);
//     if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
//         std::cerr << "Invalid address." << std::endl;
//         close(clientSocket);
//         return 1;
//     }
//     if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
//         std::cerr << "Connection to server failed." << std::endl;
//         close(clientSocket);
//         return 1;
//     }
//     /* GET request for /index.html. */
//     std::string request = "GET /index.html HTTP/1.1\r\n"
//                           "Host: localhost\r\n"
//                           "\r\n";
//     if (send(clientSocket, request.c_str(), request.size(), 0) < 0) {
//         std::cerr << "Send failed." << std::endl;
//         close(clientSocket);
//         return 1;
//     }
//     /* Read the response. */
//     char buffer[4096];
//     memset(buffer, 0, sizeof(buffer));
//     int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
//     if (bytesReceived < 0) {
//         std::cerr << "Receive failed." << std::endl;
//     } else {
//         std::cout << "\n[Client] Received response:\n" << buffer << std::endl;
//     }
//     // close(clientSocket);

//     /* Sleep and join the server thread. */
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     serverThread.join();
//     return 0;
// }

// #include <iostream>
// #include <thread>
// #include <chrono>
// #include <vector>
// #include <cstring>
// #include <sys/socket.h>
// #include <arpa/inet.h>
// #include <unistd.h>

// int main(int argc, char **argv) {
//     if (argc != 2) {
//         std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
//         return 1;
//     }
//     const char* configFile = argv[1];
//     /* Start the server in a separate thread to simulate client/server interaction */
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
//         // Optionally, display parsed config
//         // configParse.display();
//         std::vector<ServerBlock> serverBlocks = configParse.getServers();
//         ErrorHandler::getInstance().logError("Adding custom error pages...");
//         for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
//             it->setErrorPages(configParse._errorPages);
//         }
//         ServerLoop serverLoop(serverBlocks);
//         serverLoop.startServer();
//     });
//     /* Wait time to ensure server is up */
//     std::this_thread::sleep_for(std::chrono::seconds(2));

//     auto simulateClientRequest = [](const std::string &requestDescription, const std::string &requestData) {
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

//     // Simulate a static resource request (for /index.html)
//     std::string staticRequest = "GET /index.html HTTP/1.1\r\n"
//                                 "Host: localhost\r\n"
//                                 "\r\n";
//     simulateClientRequest("Requesting static resource (/www/index.html)", staticRequest);

//     // Simulate a CGI request (for /cgi-bin/test.php)
//     std::string cgiRequest = "GET /cgi-bin/test.php HTTP/1.1\r\n"
//                              "Host: localhost\r\n"
//                              "\r\n";
//     simulateClientRequest("Requesting CGI resource (/cgi-bin/test.php)", cgiRequest);

//     /* Sleep and join the server thread. */
//     std::this_thread::sleep_for(std::chrono::seconds(2));
//     serverThread.join();
//     return 0;
// }

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        return 1;
    }
    const char* configFile = argv[1];

    /* Start the server in a separate thread to simulate client/server interaction */
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
        // Optionally, display parsed configuration
        // configParse.display();
        std::vector<ServerBlock> serverBlocks = configParse.getServers();
        ErrorHandler::getInstance().logError("Adding custom error pages...");
        for (std::vector<ServerBlock>::iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it) {
            it->setErrorPages(configParse._errorPages);
        }
        ServerLoop serverLoop(serverBlocks);
        serverLoop.startServer();
    });

    /* Wait to ensure server is up */
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Helper lambda to simulate a client request.
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

    /* Sleep and join the server thread. */
    std::this_thread::sleep_for(std::chrono::seconds(2));
    serverThread.join();
    return 0;
}
