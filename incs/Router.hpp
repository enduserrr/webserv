/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:44 by asalo             #+#    #+#             */
/*   Updated: 2025/04/09 14:16:18 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "CgiHandler.hpp"
#include "StaticHandler.hpp"
#include "Logger.hpp"

class Router {
private:
    std::map<std::string, std::string> _resourceMap;
    Router();

public:
    static Router& getInstance();
    ~Router();

    void addRedirectionRule(const std::string& resourceUri, const std::string& newLocation);
    std::string routeRequest(HttpRequest &req, int clientFd);
    void sendResponse(int clientSocket, const std::string &response);
    std::string findFromMap(const std::string& key);
};

#endif

