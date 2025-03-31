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
    Logger::getInstance().logLevel("INFO", "Signal (" + std::to_string(signum) + ")  received, stopping server...", 0);
    if (g_serverLoop)
        g_serverLoop->stop();
}

bool preCheckAndSet(int ac, char **av, std::string &fileName) {
    if (ac > 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        std::cout << "or to run with default settings: ./webserv" << std::endl;
        return false;
    }
    if (ac == 2) {
        fileName = av[1]; 
        Logger::getInstance().logLevel("INFO", "Argument detected, checking if valid file", 0);
    } else { 
        fileName = DEF_CONF;
        Logger::getInstance().logLevel("INFO", "No arguments detected, using default.conf", 0);
    }
    return true; 
}

int main(int ac, char **av) {
    signal(SIGPIPE, SIG_IGN);
    std::string fileName; 
    if (!preCheckAndSet(ac, av, fileName)) 
        return 1; 
    ConfParser configParse(fileName);
    try {
        configParse.fileValidation();
        configParse.parseFile();
    } catch (const std::exception &e) {
        Logger::getInstance().logLevel("SYS_ERROR", e.what(), 0);
        Logger::getInstance().logLevel("INFO", "Parsing failed, closing program..", 0);
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
