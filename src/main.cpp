/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 11:30:12 by asalo             #+#    #+#             */
/*   Updated: 2025/04/25 09:44:08 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Global.hpp"
#include "Logger.hpp"
#include "ConfParser.hpp"
#include "ServerBlock.hpp"
#include "ServerLoop.hpp"

ServerLoop *g_serverLoop = nullptr;

void signalHandler(int signum) {
    Logger::getInstance().logLevel("SYSTEM", "Signal (" + std::to_string(signum) + ")  received, stopping server...", 0);
    if (g_serverLoop)
        g_serverLoop->stop();
}

/**
 * @brief Creates a log file with a timestamp-based name in the logs/ directory.
 *
 * Creates the logs/ directory if it doesn't exist and generates a log file
 * named ddmmyy-hhmmss.log (e.g., 250425-143022.log).
 *
 * @return The full path to the created log file (e.g., logs/250425-143022.log).
 */
std::string createLogFile() {
    // Create logs/ directory if it doesn't exist
    const std::string logDir = "logs";
    if (mkdir(logDir.c_str(), 0755) != 0 && errno != EEXIST) {
        Logger::getInstance().logLevel("SYSTEM", "Failed to create logs directory: " + std::string(strerror(errno)), 0);
        return "";
    }

    // Generate timestamp-based filename (ddmmyy-hhmmss.log)
    std::time_t now = std::time(nullptr);
    std::stringstream ss;
    ss << logDir << "/" << std::put_time(std::localtime(&now), "%d%m%y-%H%M%S.log");
    return ss.str();
}

/**
 * @brief Validates program arguments and sets the configuration file path.
 *
 * Accepts zero or one argument. If one is provided, it's used as the config file path.
 * If none, falls back to a default config file.
 *
 * @param ac Argument count.
 * @param av Argument values.
 * @param fileName Output parameter to store the config file path.
 * @return false if too many arguments are provided, true otherwise.
 */

bool preCheckAndSet(int ac, char **av, std::string &fileName) {
    if (ac > 2) {
        std::cout << "Usage: ./webserv <configfile.conf>" << std::endl;
        std::cout << "or to run with default settings: ./webserv" << std::endl;
        return false;
    }
    if (ac == 2) {
        fileName = av[1];
        Logger::getInstance().logLevel("SYSTEM", "Argument detected, checking if valid file", 0);
    } else {
        fileName = DEF_CONF;
        Logger::getInstance().logLevel("SYSTEM", "No arguments detected, using default.conf", 0);
    }
    return true;
}

int main(int ac, char **av) {
    signal(SIGPIPE, SIG_IGN);

    // Create log file and configure Logger
    std::string logFilePath = createLogFile();
    if (!logFilePath.empty()) {
        Logger::getInstance().setLogFile(logFilePath);
    } else {
        Logger::getInstance().logLevel("SYSTEM", "Failed to create log file, using console only", 0);
    }

    std::string fileName;
    if (!preCheckAndSet(ac, av, fileName))
        return 1;
    ConfParser configParse(fileName);
    try {
        configParse.fileValidation();
        configParse.parseFile();
    } catch (const std::exception &e) {
        Logger::getInstance().logLevel("SYSTEM", e.what(), 0);
        Logger::getInstance().logLevel("SYSTEM", "Parsing failed, closing program..", 0);
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
