/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:22:34 by asalo             #+#    #+#             */
/*   Updated: 2025/03/31 10:43:13 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERLOOP_HPP
# define SERVERLOOP_HPP

#include "ServerBlock.hpp"
#include "Logger.hpp"
#include "HttpParser.hpp"
#include "CgiHandler.hpp"
#include "UploadHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpParser.hpp"
#include "Router.hpp"
#include "Webserver.hpp"
#include "ClientSession.hpp"
#include <vector>
#include <poll.h>// Pollfd
#include <map>// For storing client data
#include <ctime>

#define GC     "\033[3;90m"
#define WB     "\033[1;97m"
#define GB     "\033[1;92m"
#define RB     "\033[1;91m"
#define RES    "\033[0m"

#define MAX_CLIENTS 100

class ServerLoop {
    private:
        std::map<int, ClientSession> _clients;

        std::vector<int>            _serverSockets;
        std::vector<int>            _boundPorts;
        std::map<int, ServerBlock>  _portToBlock; // Maps port -> ServerBlock (first assigned block)

        std::vector<ServerBlock>    _serverBlocks; // Config data
        std::vector<struct pollfd>  _pollFds; // Fd's for polling
        time_t                      _startUpTime;
        bool                        _run;
        // int                         _clientCount;

        void    removeClient(int clientFd);
        void    setupServerSockets();
        void    acceptNewConnection(int serverSocket);
        void    handleClientRequest(int clientSocket);
        void    sendResponse(int clientSocket, const std::string &response);
        void    handleMultipartUpload(ClientSession &client);

    public:
        ServerLoop();
        ServerLoop(const std::vector<ServerBlock> &serverBlocks);
        ~ServerLoop();

        void    startServer();
        void    closeServer();
        bool    hasTimedOut();
        bool    serverFull();
        void    stop() {
            _run = false;
        }
        // void test();
};

#endif
