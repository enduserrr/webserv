/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:51 by asalo             #+#    #+#             */
/*   Updated: 2025/04/27 15:22:14 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CLIENTSESSION_HPP
#define CLIENTSESSION_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "ServerBlock.hpp"
#include "Logger.hpp"


class ClientSession {
    private:
        size_t _requestCount;
        time_t _lastRequestTime;
        std::string _serverName;

    public:
        int fd;
        HttpRequest request;
        std::string buffer;
        ServerBlock _block;

        ClientSession();
        ClientSession(int clientFd);
        ~ClientSession();

    void        removeClient();
    bool        requestLimiter();
    void        setServerName(std::string name);
    std::string getServerName();
};

#endif
