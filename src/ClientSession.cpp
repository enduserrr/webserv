/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:32 by asalo             #+#    #+#             */
/*   Updated: 2025/04/30 11:38:47 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "ClientSession.hpp"

ClientSession::ClientSession() : _requestCount(0),
                _windowStartTime(time(nullptr)), _serverName("unknown"), fd(-1) {}

ClientSession::ClientSession(int clientFd) : _requestCount(0), _windowStartTime(time(nullptr)), _serverName("unknown"), fd(clientFd) {}

ClientSession::~ClientSession() {}

/**
 * @brief   Check if client count has reached the limit within the given time window.
 *          If the window has passed reset the window start time and the request count
 */
bool ClientSession::requestLimiter() {
    time_t now = time(nullptr);

    if (now - _windowStartTime >= WINDOW_DURATION) {
        _windowStartTime = now;
        _requestCount = 0;
    }

    if (_requestCount >= MAX_REQUESTS_PER_WINDOW) {
        return true; // Limit exceeded
    }

    _requestCount++;
    return false; // Increment as limit not exceeded
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
