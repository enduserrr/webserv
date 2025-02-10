/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:38:38 by asalo             #+#    #+#             */
/*   Updated: 2025/02/10 11:16:45 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Methods.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat

Methods::Methods() {}

Methods::~Methods() {}

#include <sstream>
#include <dirent.h>
#include <string>

static void replaceAll(std::string &str, const std::string &from, const std::string &to) {
    size_t startPos = 0;
    while((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
}

std::string Methods::generateDirectoryListing(const std::string &directoryPath, const std::string &uri) {
    // Load the template file
    std::ifstream templateFile("listing.html");
    if (!templateFile) {
        // If the template file isn't available, fallback to simple generation.
        return "";
    }
    std::ostringstream tmplStream;
    tmplStream << templateFile.rdbuf();
    std::string templateHtml = tmplStream.str();

    // Generate the list items from the directory contents.
    std::ostringstream itemsStream;
    DIR *dir = opendir(directoryPath.c_str());
    if (!dir) {
        return "";
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;
        itemsStream << "<li><a href=\"" << uri;
        if (uri.back() != '/')
            itemsStream << "/";
        itemsStream << name << "\">" << name << "</a></li>\n";
    }
    closedir(dir);
    std::string itemsHtml = itemsStream.str();

    // Replace placeholders in the template.
    // Replace "{{title}}" with "Index of <uri>".
    std::string title = "Index of " + uri;
    replaceAll(templateHtml, "{{title}}", title);

    // Replace "{{items}}" with the generated list.
    replaceAll(templateHtml, "{{items}}", itemsHtml);

    return templateHtml;
}

std::string Methods::mGet(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string basePath = "www";
    std::string filePath = basePath + uri;

    struct stat st;
    bool isDirectory = false;
    if (stat(filePath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
        isDirectory = true;
    }

    // If the URI ends with '/', it's a directory request.
    if (!uri.empty() && uri.back() == '/') {
        if (isDirectory && req.getAutoIndex()) {
            // Generate directory listing using the template.
            std::string listing = generateDirectoryListing(filePath, uri);
            if (!listing.empty()) {
                std::ostringstream responseStream;
                responseStream << "HTTP/1.1 200 OK\r\n"
                               << "Content-Length: " << listing.size() << "\r\n"
                               << "Content-Type: text/html\r\n"
                               << "\r\n"
                               << listing;
                return responseStream.str();
            } else {
                std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
                response += ErrorHandler::getInstance().getErrorPage(500);
                return response;
            }
        } else {
            // If auto indexing is disabled, serve the default file.
            filePath += "index.html";
        }
    }

    // Check if the file exists.
    if (stat(filePath.c_str(), &st) != 0) {
        std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
        std::cerr << "Unable to fetch the requested resource." << std::endl;
        response += ErrorHandler::getInstance().getErrorPage(404);
        return response;
    }

    // Open the file for reading.
    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n";
        response += ErrorHandler::getInstance().getErrorPage(500);
        return response;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileContent = ss.str();

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
