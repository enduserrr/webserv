/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 11:30:12 by asalo             #+#    #+#             */
/*   Updated: 2025/03/16 13:03:15 by asalo            ###   ########.fr       */
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
    signal(SIGPIPE, SIG_IGN);
    if (ac != 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        return 1;
    }
    std::cout << "\nOpening program...\n" << std::endl;

    ConfParser configParse(av[1]);
    try {
        configParse.fileValidation();
        configParse.parseFile();
        // configParse.display();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "\nShutting down program..." << std::endl;
        return 1;
    }
    std::vector<ServerBlock> serverBlocks = configParse.getServers();
    ServerLoop serverLoop(serverBlocks);
    g_serverLoop = &serverLoop;

    // Sigaction instead of signal() to register the handler without SA_RESTART.
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    // Do not set SA_RESTART so that blocking calls are interrupted
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    serverLoop.startServer();
    return 0;
}
