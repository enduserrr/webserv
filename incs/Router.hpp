/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:44 by asalo             #+#    #+#             */
/*   Updated: 2025/02/13 12:38:11 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <string>
#include <map>
#include <sstream>
#include "HttpRequest.hpp"
#include "CgiHandler.hpp"
#include "StaticHandler.hpp"
#include "ErrorHandler.hpp"

class Router {
private:
    // Mapping of old URIs to new URIs for relocated resources.
    std::map<std::string, std::string> redirectionMap;

public:
    Router();
    ~Router();

    void addRedirectionRule(const std::string& oldUri, const std::string& newUri);

    // std::string routeRequest(HttpRequest &req);
    std::string routeRequest(HttpRequest &req, int clientFd);
    void    sendResponse(int clientSocket, const std::string &response);
};

#endif

