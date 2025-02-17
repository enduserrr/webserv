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

ClientSession::ClientSession() : fd(-1) {}

ClientSession::ClientSession(int clientFd) : fd(clientFd) {}

ClientSession::~ClientSession() {}

void ClientSession::removeClient() {
    if (fd != -1) {
        close(fd);
    }
}
