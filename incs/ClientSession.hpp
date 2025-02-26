/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:51 by asalo             #+#    #+#             */
/*   Updated: 2025/02/17 11:34:23 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CLIENTSESSION_HPP
#define CLIENTSESSION_HPP

#include "HttpRequest.hpp"
#include "ServerBlock.hpp"
#include "ErrorHandler.hpp"
#include "Libs.hpp"
// #include <string>

class ClientSession {
    private:
        size_t _requestCount; 
        time_t _lastRequestTime; 

    public:
        int fd;
        HttpRequest request;
        std::string buffer;
        ServerBlock _block;

        ClientSession();
        ClientSession(int clientFd);
        ~ClientSession();

    void removeClient();
    bool requestLimiter(); 
};

#endif
