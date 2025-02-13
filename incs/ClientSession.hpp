/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientSession.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 12:18:51 by asalo             #+#    #+#             */
/*   Updated: 2025/02/13 12:41:36 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CLIENTSESSION_HPP
#define CLIENTSESSION_HPP

#include "HttpRequest.hpp"
#include "ErrorHandler.hpp"
#include "Libs.hpp"
// #include <string>

class ClientSession {
    private:

    public:
        int fd;
        HttpRequest request;
        std::string buffer;

        ClientSession();
        ClientSession(int clientFd);
        ~ClientSession();

    void closeConnection();
};

#endif
