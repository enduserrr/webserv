/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:22:34 by asalo             #+#    #+#             */
/*   Updated: 2025/02/04 11:36:04 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERLOOP_HPP
# define SERVERLOOP_HPP

#include "ServerBlock.hpp"
#include "ErrorHandler.hpp"
#include "HttpParser.hpp"
#include "CgiHandler.hpp"
#include "UploadHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpParser.hpp"
#include "Webserver.hpp"
#include <vector>
#include <poll.h>// Pollfd
#include <map>// For storing client data
#include <ctime>

#define GC     "\033[3;90m"
#define WB     "\033[1;97m"
#define GB     "\033[1;92m"
#define RB     "\033[1;91m"
#define RES    "\033[0m"

class ServerLoop {
    private:
        std::vector<ServerBlock> _serverBlocks; // Config data
        std::vector<struct pollfd> _pollFds; // Fd's for polling
        std::map<int, std::string> _clientData; // Store client data (like requests)
        time_t _startUpTime; // Server starting timestamp
        // HttpParser _httpParser;

        void setupServerSockets();
        void acceptNewConnection(int serverSocket);
        void handleClientRequest(int clientSocket);
        void sendResponse(int clientSocket, const std::string &response);
        // std::string routeRequest(HttpParser &parser);
        std::string routeRequest(HttpRequest &req);

    public:
        ServerLoop();
        ServerLoop(const std::vector<ServerBlock> &serverBlocks);
        ~ServerLoop();

        void startServer();
        void closeServer();
        bool hasTimedOut();  // Check if the server has timed out

};

#endif
