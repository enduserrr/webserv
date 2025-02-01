/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:41:08 by asalo             #+#    #+#             */
/*   Updated: 2025/02/01 11:41:12 by asalo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/StaticHandler.hpp"
#include "../incs/ErrorHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

std::string StaticHandler::processRequest(HttpParser &parser) {
    // Assume HttpParser has a getUri() method that returns the requested URI as a string.
    std::string uri = parser.getUri();

    // Base directory for static files (you might make this configurable)
    std::string basePath = "www";
    std::string filePath = basePath + uri;

    // If the URI ends with '/', append "index.html"
    if (!uri.empty() && uri.back() == '/') {
        filePath += "index.html";
    }

    // Check if the file exists using stat()
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        // File not found; return a 404 response using the singleton ErrorHandler.
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        std::cerr << "Unable to fetch the requested resource." << std::endl;
        response += ErrorHandler::getInstance().getErrorPage(404);
        return response;
    }

    // Open the file for reading in binary mode.
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        // Error opening file; return a 500 error.
        std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
        response += ErrorHandler::getInstance().getErrorPage(500);
        return response;
    }

    // Read the file content into a string.
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileContent = ss.str();

    /* Redo to comply with the subject */
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n"
                   << "Content-Length: " << fileContent.size() << "\r\n"
                   << "Content-Type: text/html\r\n" // For simplicity; in a complete implementation, determine MIME type.
                   << "\r\n"
                   << fileContent;

    return responseStream.str();
}
