/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:38:38 by asalo             #+#    #+#             */
/*   Updated: 2025/02/24 11:08:24 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Methods.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat
#define RB     "\033[1;91m"
#define RES    "\033[0m"
#define GC     "\033[3;90m"

Methods::Methods() {}

Methods::~Methods() {}

#include <sstream>
#include <dirent.h>
#include <string>

static void replaceAll(std::string &str, const std::string &from, const std::string &to) {
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
}

std::string Methods::generateDirectoryListing(const std::string &directoryPath, const std::string &uri) {
    std::ifstream templateFile("/www/templates/folder.html"); // Use correct template path
    if (!templateFile) { // Fallback in case the template is missing
        std::cout << "FALLBACK FILE" << std::endl;
        std::ostringstream fallback;
        fallback << "<html><head><title>Index of " << uri << "</title></head><body>"
                 << "<h1>Index of " << uri << "</h1><ul>";
        DIR *dir = opendir(directoryPath.c_str());
        if (!dir) return "";
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name == "." || name == "..")
                continue;
            fallback << "<li><a href=\"" << uri;
            if (uri.back() != '/')
                fallback << "/";
            fallback << name << "\">" << name << "</a></li>\n";
        }
        closedir(dir);
        fallback << "</ul></body></html>";
        return fallback.str();
    }

    // Read template file into string
    std::ostringstream tmplStream;
    tmplStream << templateFile.rdbuf();
    std::string templateHtml = tmplStream.str();

    std::ostringstream itemsStream; // Generate the list items from the directory contents
    DIR *dir = opendir(directoryPath.c_str());
    if (!dir) {
        return "";
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue;

        std::string filePath = uri;
        if (uri.back() != '/')
            filePath += "/";
        filePath += name;

        std::string fileItem = "<li>" + name;

        // File preview button for supported formats
        if (name.find(".jpg") != std::string::npos || name.find(".jpeg") != std::string::npos ||
            name.find(".png") != std::string::npos || name.find(".gif") != std::string::npos) {
            fileItem += " <button class='view-btn' onclick='previewFile(\"" + filePath + "\")'>View</button>";
        } else if (name.find(".mp4") != std::string::npos || name.find(".webm") != std::string::npos ||
                   name.find(".ogg") != std::string::npos) {
            fileItem += " <button class='view-btn' onclick='previewFile(\"" + filePath + "\")'>Play</button>";
        }

        // Delete button
        fileItem += " <a href='/delete?file=" + name + "' class='delete-btn'>Delete</a>";

        fileItem += "</li>\n";
        itemsStream << fileItem;
    }
    closedir(dir);

    std::string itemsHtml = itemsStream.str();
    std::string title = "Uploads - " + uri;

    // Replace placeholders in folder.html
    replaceAll(templateHtml, "{{title}}", title);
    replaceAll(templateHtml, "{{files}}", itemsHtml);

    return templateHtml;
}


/* // std::string Methods::generateDirectoryListing(const std::string &directoryPath, const std::string &uri) {
//     std::ifstream templateFile("www/uploads/folder.html"); // listing.html template from www/template
//     if (!templateFile) { // Else, make a simple HTML page
//         std::cout << "FALLBACK FILE" << std::endl;
//         std::ostringstream fallback;
//         fallback << "<html><head><title>Index of " << uri << "</title></head><body>"
//                  << "<h1>Index of " << uri << "</h1><ul>";
//         DIR *dir = opendir(directoryPath.c_str());
//         if (!dir) return "";
//         struct dirent *entry;
//         while ((entry = readdir(dir)) != nullptr) {
//             std::string name = entry->d_name;
//             if (name == "." || name == "..")
//                 continue;
//             fallback << "<li><a href=\"" << uri;
//             if (uri.back() != '/')
//                 fallback << "/";
//             fallback << name << "\">" << name << "</a></li>\n";
//         }
//         closedir(dir);
//         fallback << "</ul></body></html>";
//         return fallback.str();
//     }

//     std::ostringstream tmplStream;
//     tmplStream << templateFile.rdbuf();
//     std::string templateHtml = tmplStream.str();

//     std::ostringstream itemsStream; // Generate the list items from the directory contents.
//     DIR *dir = opendir(directoryPath.c_str());
//     if (!dir) {
//         return "";
//     }
//     struct dirent *entry;
//     while ((entry = readdir(dir)) != nullptr) {
//         std::string name = entry->d_name;
//         if (name == "." || name == "..")
//             continue;
//         itemsStream << "<li><a href=\"" << uri;
//         if (uri.back() != '/')
//             itemsStream << "/";
//         itemsStream << name << "\">" << name << "</a></li>\n";
//     }
//     closedir(dir);
//     std::string itemsHtml = itemsStream.str();

//     std::string title = "Index of " + uri;
//     replaceAll(templateHtml, "{{title}}", title); // Replace placeholders in the template with dir listings
//     replaceAll(templateHtml, "{{items}}", itemsHtml);

//     return templateHtml;
// } */

std::string Methods::mGet(HttpRequest &req) {
    std::string uri = req.getUri();
    // Use the root specified in the request (populated from ServerBlock or Location)
    std::string basePath = req.getRoot();
    std::string filePath = basePath + uri;

    std::cout << GC << "ROOT: " << req.getRoot() << "\nAUTO-INDEX: " << req.getAutoIndex() << RES << std::endl;
    struct stat st;
    bool isDirectory = false;
    if (stat(filePath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
        isDirectory = true;
    }
    if (!uri.empty() && uri.back() == '/') {// If the URI ends with '/', it's a directory request.
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
                return ErrorHandler::getInstance().getErrorPage(500);
            }
        } else {
            filePath += "index.html";// Autoindex is off => return the index page
        }
    }
    if (stat(filePath.c_str(), &st) != 0)// Check if the file exists.
        return ErrorHandler::getInstance().getErrorPage(404);

    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);// Open the file for reading in binary mode.
    if (!file)
        return ErrorHandler::getInstance().getErrorPage(500);

    std::ostringstream ss;
    ss << file.rdbuf();// Read the file content.
    std::string fileContent = ss.str();

    std::ostringstream responseStream;// Build the HTTP response.
    responseStream << "HTTP/1.1 200 OK\r\n"
                   << "Content-Length: " << fileContent.size() << "\r\n"
                   << "Content-Type: text/html\r\n"
                   << "\r\n"
                   << fileContent;
    return responseStream.str();
}


std::string Methods::mPost(HttpRequest &req) {
    if (req.getBody().empty())
        return ErrorHandler::getInstance().getErrorPage(404);

    UploadHandler uploadHandler;
    std::string uploadedFilePath = uploadHandler.uploadReturnPath(req);

    if (uploadedFilePath.find("HTTP/1.1") == 0)
        // return uploadedFilePath;
        ErrorHandler::getInstance().getErrorPage(500);
    // Read the upload-success.html template
    std::ifstream file("/home/asalo/Code/enduserrr/c++/webserv/www/templates/upload-success.html");
    if (!file.is_open())
        return ErrorHandler::getInstance().getErrorPage(500);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string htmlContent = buffer.str();
    file.close();
    replaceAll(htmlContent, "{{file_path}}", uploadedFilePath);

    std::ostringstream uploadResponse;
    uploadResponse << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << htmlContent;
    return uploadResponse.str();
}

std::string Methods::mDelete(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string basePath = "www";//change this
    std::string filePath = basePath + uri;

    // Allow deletion only if the file is in the /uploads directory
    if (filePath.find("/uploads/") == std::string::npos)
        return ErrorHandler::getInstance().getErrorPage(403);

    // Check if the file exists
    struct stat st;
    if (stat(filePath.c_str(), &st) != 0)
        return ErrorHandler::getInstance().getErrorPage(404);

    // Attempt delete
    if (remove(filePath.c_str()) != 0)
        return ErrorHandler::getInstance().getErrorPage(500);

    std::ostringstream deleteResponse;
    deleteResponse << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                    << "<html><body><h1>Delete Successful</h1>"
                    << "<p>The file has been deleted successfully.</p></body></html>";
    return deleteResponse.str();
}
