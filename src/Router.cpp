/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:16 by asalo             #+#    #+#             */
/*   Updated: 2025/02/13 12:49:56 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Router.hpp"
#include <iostream>


Router::Router() {}

Router::~Router() {}

void Router::addRedirectionRule(const std::string& oldUri, const std::string& newUri) {
    redirectionMap[oldUri] = newUri;
}

std::string Router::routeRequest(HttpRequest &req, int clientFd) {
    std::string uri = req.getUri();

    if (redirectionMap.count(uri)) {
        std::ostringstream response;
        response << "HTTP/1.1 301 Moved Permanently\r\n";
        response << "Location: " << redirectionMap[uri] << "\r\n";
        response << "Content-Type: text/html\r\n\r\n";
        response << "<html><body><h1>301 Moved Permanently</h1>"
                 << "<p>This resource has moved to <a href=\"" << redirectionMap[uri] << "\">"
                 << redirectionMap[uri] << "</a>.</p></body></html>";

        sendResponse(clientFd, response.str());
        return response.str();
    }

    // Handle CGI or static request
    if (uri.find("/cgi-bin/") == 0 || uri.substr(uri.size()-4) == ".php") {
        CgiHandler cgiHandler;
        return cgiHandler.processRequest(req);
    } else {
        StaticHandler staticHandler;
        return staticHandler.processRequest(req);
    }
}

void    Router::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        ErrorHandler::getInstance().logError("Failed to send response to client.");
        std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                                    + ErrorHandler::getInstance().getErrorPage(500);
        send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0);
    }
}

