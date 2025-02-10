/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:16 by asalo             #+#    #+#             */
/*   Updated: 2025/02/10 11:19:50 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Router.hpp"
#include <iostream>


Router::Router() {}

Router::~Router() {}

void Router::addRedirectionRule(const std::string& oldUri, const std::string& newUri) {
    redirectionMap[oldUri] = newUri;
}

std::string Router::routeRequest(HttpRequest &req) {
    std::string uri = req.getUri();

    // Check for redirection: if the requested URI has been relocated.
    auto it = redirectionMap.find(uri);
    if (it != redirectionMap.end()) {
        // Build a 301 Moved Permanently response.
        std::ostringstream response;
        response << "HTTP/1.1 301 Moved Permanently\r\n";
        response << "Location: " << it->second << "\r\n";
        response << "Content-Type: text/html\r\n\r\n";
        response << "<html><body><h1>301 Moved Permanently</h1>"
                 << "<p>This resource has moved to <a href=\"" << it->second << "\">" << it->second << "</a>.</p>"
                 << "</body></html>";
        ErrorHandler::getInstance().logError("Redirecting " + uri + " to " + it->second);
        return response.str();
    }

    // Route request based on the URI pattern.
    // If the URI starts with "/cgi-bin/" or ends with ".php", delegate to the CGI handler.
    if (uri.find("/cgi-bin/") == 0 || (uri.size() >= 4 && uri.substr(uri.size()-4) == ".php")) {
        CgiHandler cgiHandler;
        return cgiHandler.processRequest(req);
    } else {
        // Else, treat it as a request for a static resource.
        StaticHandler staticHandler;
        return staticHandler.processRequest(req);
    }
}

