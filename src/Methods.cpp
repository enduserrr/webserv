/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:38:38 by asalo             #+#    #+#             */
/*   Updated: 2025/02/05 12:12:58 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Methods.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat

Methods::Methods() {}

Methods::~Methods() {}

std::string Methods::mGet(HttpRequest &req) {
    std::string uri = req.getUri();
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

std::string Methods::mPost(HttpRequest &req) {
    if (req.getBody().empty()) {
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        response += ErrorHandler::getInstance().getErrorPage(404);
        return response;
    }

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

std::string Methods::mDelete(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string basePath = "www";
    std::string filePath = basePath + uri;

    /* Allow deletion only if the file is in the /uploads directory */
    if (filePath.find("/uploads/") == std::string::npos) {
        std::string response = "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n";
        response += ErrorHandler::getInstance().getErrorPage(403);
        return response;
    }

    /* Check if the file exists */
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) {
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        std::cerr << "File to delete not found." << std::endl;
        response += ErrorHandler::getInstance().getErrorPage(404);
        return response;
    }

    /* Attempt delete */
    if (remove(filePath.c_str()) != 0) {
        std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
        response += ErrorHandler::getInstance().getErrorPage(500);
        return response;
    }
    std::ostringstream deleteResponse;
    deleteResponse << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                    << "<html><body><h1>Delete Successful</h1>"
                    << "<p>The file has been deleted successfully.</p></body></html>";
    return deleteResponse.str();
}
