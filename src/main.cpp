/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 11:30:12 by asalo             #+#    #+#             */
/*   Updated: 2025/01/17 11:30:13 by asalo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Webserver.hpp"

int main(int argc, char **argv) {
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
}


//debug main for http parser



// int main()
// {
//     std::string rawDataChunk =
//     "POST /upload HTTP/1.1\r\n"
//     "Host: example.com\r\n"
//     "Content-Type: application/x-www-form-urlencoded\r\n"
//     "Content-Length: 16\r\n"
//     "\r\n"
//     "name=John&age=30"
//     "GET /index.html HTTP/1.1\r\n"
//     "Host: example2.com\r\n";

//     //Ei toimi viela kahdella requestilla, mutta pilkkoo ensimmaisesta yhden kokonaisen
//     // std::string rawDataChunk2 =
//     // "Content-Type: application/secondreq\r\n"
//     // "\r\n";


//     std::istringstream input(rawDataChunk);
//     HttpParser parser;
//     parser.readFullRequest(input);
//     // std::istringstream input2(rawDataChunk2);
//     // parser.readFullRequest(input2);

//     //parser result for debuggin
//     parser.display();

//     return (0);
// }
