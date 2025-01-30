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
    for (auto &server : serverBlocks) {
        server.setErrorPages(configParse._errorPages);
    }

    ServerLoop serverLoop(serverBlocks);
    serverLoop.startServer();

    return 0;
}
