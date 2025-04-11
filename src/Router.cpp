/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:16 by asalo             #+#    #+#             */
/*   Updated: 2025/04/10 21:25:45 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Router.hpp"

Router::Router() {}

Router& Router::getInstance() {
    static Router instance;
    return instance;
}

Router::~Router() {}

std::string Router::routeRequest(HttpRequest &req) {
    std::string uri = req.getUri();

    std::string root = req.getLocation().getRoot(); 
    bool t = req.getLocation().getAutoIndex();

    std::cout << root << t << std::endl;
    std::cout << req.getAutoIndex() << std::endl;
    // Handle CGI or static request
    if (uri.find("/cgi-bin/") == 0 || (uri.size() >= 4 && uri.substr(uri.size() - 4) == ".php")) {
        CgiHandler cgiHandler;
        std::cout << REV_RED << "URI: " << req.getUri() << RES << std::endl;
        return cgiHandler.processRequest(req);
    } else {
        StaticHandler staticHandler;
        return staticHandler.processRequest(req);
    }
}
