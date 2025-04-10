/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:16 by asalo             #+#    #+#             */
/*   Updated: 2025/04/10 20:15:44 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Router.hpp"

Router::Router() {}

Router& Router::getInstance() {
    static Router instance;
    return instance;
}

Router::~Router() {}

void Router::addRedirectionRule(const std::string& resourceUri, const std::string& newLocation) {
    std::string str = findFromMap(resourceUri);
    if (str != "")
        _resourceMap[resourceUri] = newLocation; // Update or add the resource location
}

std::string Router::routeRequest(HttpRequest &req, int clientFd) {
    std::string uri = req.getUri();
    (void)clientFd;

    std::string match = findFromMap(req.getUri());
    if (match != "") {
        req.setUri(match);
    } else { Logger::getInstance().logLevel("INFO", "Router: requested resource has no match in _resourceMap", 0); }


    // Handle CGI or static request
    if (uri.find("/cgi-bin/") == 0 || (uri.size() >= 4 && uri.substr(uri.size() - 4) == ".php")) {
        CgiHandler cgiHandler;
        // std::cout << REV_RED << "URI: " << req.getUri() << RES << std::endl;
        return cgiHandler.processRequest(req);
    } else {
        StaticHandler staticHandler;
        return staticHandler.processRequest(req);
    }
}

std::string Router::findFromMap(const std::string& key) {
    if (_resourceMap.count(key)) {
        return _resourceMap[key];
    }
    return "";
}
