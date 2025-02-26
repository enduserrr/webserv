/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:32 by asalo             #+#    #+#             */
/*   Updated: 2025/02/17 11:34:23 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/ClientSession.hpp"

// ClientSession::ClientSession() {}

// ClientSession::ClientSession(int clientFd) : fd(clientFd) {}

// ClientSession::~ClientSession() {}

// void ClientSession::removeClient() {
//         close(fd);
// }

ClientSession::ClientSession() : _requestCount(0), _lastRequestTime(time(nullptr)), fd(-1) {}

ClientSession::ClientSession(int clientFd) : fd(clientFd) {}

ClientSession::~ClientSession() {}

bool ClientSession::requestLimiter() {

    time_t now = time(nullptr);  

    if (now - _lastRequestTime >= 1) {
        _lastRequestTime = now; 
        _requestCount = 0; 
    }
    if (_requestCount >= 10) 
        return true; 
    _requestCount++;
    std::cout << " counter++ : " <<_requestCount << std::endl;
    return false; 
}


void ClientSession::removeClient() {
    if (fd != -1) {
        close(fd);
    }
}
