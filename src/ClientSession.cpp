/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:32 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:27:54 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ClientSession.hpp"

ClientSession::ClientSession() : _requestCount(0), _lastRequestTime(time(nullptr)), fd(-1), 
                                _lastRecvTime(time(nullptr)) {}

ClientSession::ClientSession(int clientFd) : _requestCount(0), _lastRequestTime(time(nullptr)), 
                                            fd(clientFd), _lastRecvTime(time(nullptr)) {}

ClientSession::~ClientSession() {}

bool ClientSession::requestLimiter() {

    time_t now = time(nullptr);

    if (now - _lastRequestTime >= 0) {
        _lastRequestTime = now;
        _requestCount = 0;
    }
    if (_requestCount >= 10)
        return true;
    _requestCount++;
    return false;
}


void ClientSession::removeClient() {
    if (fd != -1) {
        close(fd);
    }
}
