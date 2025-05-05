/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:16 by asalo             #+#    #+#             */
/*   Updated: 2025/05/05 18:07:59 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Router.hpp"

Router::Router() {}

Router& Router::getInstance() {
    static Router instance;
    return instance;
}

Router::~Router() {}

/**
 * @brief   Routes the request being processed to either cgi or static handler
 */
std::string Router::routeRequest(HttpRequest &req) {
    std::string uri = req.getUri();
/*     std::cout << "==== DEBUG =====" <<std::endl; 
    std::cout << "uri: " << req.getUri() << std::endl; 
    std::cout << "Autoindex: " << req.getLocation().getAutoIndex() << std::endl; 
    std::cout << "Root: " << req.getLocation().getRoot() << std::endl; 
    std::cout << "UploadStore: " << req.getLocation().getUploadStore() << std::endl;
    std::cout << "Allowed Methods: " << req.getLocation().getAllowedMethods()[0] << " "<< req.getLocation().getAllowedMethods()[1] << std::endl;
    std::cout << "==== DEBUG END=====" <<std::endl; */

    if (uri.find("/cgi-bin/") == 0 || (uri.size() >= 4 && uri.substr(uri.size() - 4) == ".php")) {
        CgiHandler cgiHandler;
        return cgiHandler.processRequest(req);
    } else {
        StaticHandler staticHandler;
        return staticHandler.processRequest(req);
    }
}
