/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:32 by asalo             #+#    #+#             */
/*   Updated: 2025/04/27 15:33:06 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ClientSession.hpp"

ClientSession::ClientSession() : _requestCount(0), _lastRequestTime(time(nullptr)), fd(-1) {
    _serverName = "unknown";
}

ClientSession::ClientSession(int clientFd) : _requestCount(0), _lastRequestTime(time(nullptr)), fd(clientFd) {}

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

void ClientSession::setServerName(std::string name) {
    _serverName = name;
}

std::string ClientSession::getServerName() {
    return _serverName;
}
