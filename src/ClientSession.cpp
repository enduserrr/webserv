/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:32 by asalo             #+#    #+#             */
/*   Updated: 2025/02/13 12:42:31 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/ClientSession.hpp"

// ClientSession::ClientSession() {}

// ClientSession::ClientSession(int clientFd) : fd(clientFd) {}

// ClientSession::~ClientSession() {}

// void ClientSession::closeConnection() {
//         close(fd);
// }

ClientSession::ClientSession() : fd(-1) {}

ClientSession::ClientSession(int clientFd) : fd(clientFd) {}

ClientSession::~ClientSession() {}

void ClientSession::closeConnection() {
    if (fd != -1) {
        close(fd);
    }
}
