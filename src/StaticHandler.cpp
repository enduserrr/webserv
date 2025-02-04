/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:41:08 by asalo             #+#    #+#             */
/*   Updated: 2025/02/04 11:16:53 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/StaticHandler.hpp"
#include "../incs/ErrorHandler.hpp"
#include "../incs/UploadHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

std::string StaticHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string method = req.getMethod();

    /* If this is a POST request with a body, treat it as a file upload */
    if (method == "POST" && !req.getBody().empty()) {
        UploadHandler uploadHandler;
        std::string uploadedFilePath = uploadHandler.uploadReturnPath(req);
        /* If the returned string starts with "HTTP/1.1", assume an error occurred */
        if (uploadedFilePath.find("HTTP/1.1") == 0) {
            return uploadedFilePath;
        }
        std::ostringstream uploadResponse;
        uploadResponse << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                       << "<html><body><h1>Upload Successful</h1>"
                       << "<p>Your file has been saved as " << uploadedFilePath << ".</p>"
                       << "</body></html>";
        return uploadResponse.str();
    }

    /* Otherwise, treat the request as one for a static resource */
    std::string basePath = "www";
    std::string filePath = basePath + uri;
    /* If the URI ends with '/', append "index.html" */
    if (!uri.empty() && uri.back() == '/') {
        filePath += "index.html";
    }

    /* Check if the file exists */
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        std::cerr << "Unable to fetch the requested resource." << std::endl;
        response += ErrorHandler::getInstance().getErrorPage(404);
        return response;
    }

    /* Open the file for reading in binary mode */
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
        response += ErrorHandler::getInstance().getErrorPage(500);
        return response;
    }

    /* Read the file content */
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileContent = ss.str();

    /* Build the HTTP response */
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n"
                   << "Content-Length: " << fileContent.size() << "\r\n"
                   << "Content-Type: text/html\r\n"
                   << "\r\n"
                   << fileContent;

    return responseStream.str();
}

