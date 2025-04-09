/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:16 by asalo             #+#    #+#             */
/*   Updated: 2025/04/09 14:17:05 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Router.hpp"

Router::Router() {
    _resourceMap["/index.html"] = "/index.html";
    _resourceMap["/upload.html"] = "/upload.html";
    _resourceMap["/files.html"] = "/files.html";
    _resourceMap["favicon.ico"] = "/favicon.ico";
    _resourceMap["guestbook.html"] = "/guestbook.html";
    // _resourceMap["style.css"] = "/style.css";
    _resourceMap["/upload_success.html"] = "/upload_success.html";
    _resourceMap["/welcome.php"] = "/cgi-bin/welcome.php";
    _resourceMap["/guestbook.php"] = "/cgi-bin/guestbook.php";
    _resourceMap["/guestbook_display.php"] = "/cgi-bin/guestbook_display.php";
    _resourceMap["/comments.txt"] = "/cgi-bin/comments.txt";
    // _resourceMap[""] = "";
    // NO NEED FOR ERROR PAGES RIGHT?????
}

Router& Router::getInstance() {
    static Router instance;
    return instance;
}

Router::~Router() {}

void Router::addRedirectionRule(const std::string& resourceUri, const std::string& newLocation) {
    _resourceMap[resourceUri] = newLocation; // Update or add the resource location
}

std::string Router::routeRequest(HttpRequest &req, int clientFd) {
    std::string uri = req.getUri();
    (void)clientFd;
    // Check if the requested URI is a known resource with a different location
    // if (_resourceMap.count(uri) && _resourceMap[uri] != uri) {
    //     std::ostringstream response;
    //     response << MOVED << "Location: " << _resourceMap[uri] << "\r\n"
    //              << "Content-Type: text/html\r\n\r\n"
    //              << "<html><body><p>This resource has moved to <a href=\"" << _resourceMap[uri] << "\">"
    //              << _resourceMap[uri] << "</a>.</p></body></html>";
    //     sendResponse(clientFd, response.str());
    //     return response.str();
    // }

    // Handle CGI or static request
    if (uri.find("/cgi-bin/") == 0 || (uri.size() >= 4 && uri.substr(uri.size() - 4) == ".php")) {
        CgiHandler cgiHandler;
        return cgiHandler.processRequest(req);
    } else {
        StaticHandler staticHandler;
        return staticHandler.processRequest(req);
    }
}

void Router::sendResponse(int clientSocket, const std::string &response) {
    if (send(clientSocket, response.c_str(), response.size(), 0) < 0) {
        Logger::getInstance().logLevel("SYSTEM", "Failed to send response to client.", 1);
        std::string errorResponse = INTERNAL + Logger::getInstance().getErrorPage(500);
        send(clientSocket, errorResponse.c_str(), errorResponse.size(), 0); // Fallback send
    }
}

std::string Router::findFromMap(const std::string& key) {
    if (_resourceMap.count(key)) {
        return _resourceMap[key];
    }
    return "";
}
