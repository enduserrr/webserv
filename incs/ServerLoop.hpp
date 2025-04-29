/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerLoop.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:22:34 by asalo             #+#    #+#             */
/*   Updated: 2025/04/10 21:34:56 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef SERVERLOOP_HPP
# define SERVERLOOP_HPP

#include "Global.hpp"
#include "ServerBlock.hpp"
#include "Logger.hpp"
#include "HttpParser.hpp"
#include "CgiHandler.hpp"
#include "UploadHandler.hpp"
#include "HttpRequest.hpp"
#include "HttpParser.hpp"
#include "Router.hpp"
#include "ClientSession.hpp"

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

        std::map<int, time_t>       _clientLastActivity; // Client last activity time
        const time_t                _clientTimeoutDuration = 60;

        void    removeClient(int clientFd);
        void    setupServerSockets();
        void    acceptNewConnection(int serverSocket);
        void    handleClientRequest(int clientSocket);
        void    sendResponse(int clientSocket, const std::string &response);
        void    checkClientTimeouts();
        bool    MatchBlockToRequest(const std::string &host, int clientSocket, HttpParser &parser);

    public:
        ServerLoop();
        ServerLoop(const std::vector<ServerBlock> &serverBlocks);
        ~ServerLoop();

        void    startServer();
        void    closeServer();
        bool    serverFull();
        void    stopServer();
        void    stop();
        bool    hasTimedOut();
};

#endif
