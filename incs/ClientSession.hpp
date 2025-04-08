/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:51 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:09:10 by asalo            ###   ########.fr       */
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

    public:
        int fd;
        HttpRequest request;
        std::string buffer;
        ServerBlock _block;
        time_t _lastRecvTime;

        ClientSession();
        ClientSession(int clientFd);
        ~ClientSession();

    void removeClient();
    bool requestLimiter();
};

#endif
