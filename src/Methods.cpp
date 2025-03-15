/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:38:38 by asalo             #+#    #+#             */
/*   Updated: 2025/03/15 11:54:56 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Methods.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat
#include <unistd.h> //Access<cstring>
#include <cstring>
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
    std::ifstream templateFile("www/templates/listing.html"); // Use the listing template from www/templates
    if (!templateFile) { // Fallback in case the template is missing
        std::cout << "FALLBACK FILE" << std::endl;
        std::ostringstream fallback;
        fallback << "<html><head><title>Index of " << uri << "</title></head><body>"
                 << "<h1>Index of " << uri << "</h1><ul>";
        DIR *dir = opendir(directoryPath.c_str());
        if (!dir) {
            return "";
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string name = entry->d_name;
            if (name == "." || name == "..")
                continue ;
            fallback << "<li><a href=\"" << uri;
            if (uri.back() != '/')
                fallback << "/";
            fallback << name << "\">" << name << "</a>"
                     << " <a href='/delete?file=" << name << "' class='delete-btn'>Delete</a></li>\n";
        }
        closedir(dir);
        fallback << "</ul></body></html>";
        return fallback.str();
    }

    std::ostringstream tmplStream;
    tmplStream << templateFile.rdbuf();
    std::string templateHtml = tmplStream.str();

    std::ostringstream itemsStream; // Generate the list items from the directory contents.
    DIR *dir = opendir(directoryPath.c_str());
    if (!dir) {
        return "";
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..")
            continue ;

        std::string filePath = uri;
        if (uri.back() != '/')
            filePath += "/";
        filePath += name;

        std::string fileItem = "<li>"
                       + std::string("<a href=\"") + filePath + "\">" + name + "</a>"
                       + " <button class='delete' data-target='/delete?file=" + name + "' data-method='DELETE'>Delete</button>"
                       + "</li>\n";
        itemsStream << fileItem;
    }
    closedir(dir);

    std::string itemsHtml = itemsStream.str();
    std::string title = "Index of " + uri;
    replaceAll(templateHtml, "{{title}}", title); // Replace placeholder for title.
    replaceAll(templateHtml, "{{items}}", itemsHtml); // Replace placeholder for list items.

    return templateHtml;
}

std::string Methods::mGet(HttpRequest &req) {
    std::string uri = req.getUri();
    if (uri.length() >= 4 && uri.substr(uri.length() - 4) == ".ico") {
        std::string newUri = req.getHeader("Referer");
        if (newUri.find("/uploads/") != std::string::npos) { // Fixed condition
            uri = "/uploads/";
        }
    }
    std::cout << RB << "URI: " << uri << RES << std::endl;
    std::string basePath = req.getRoot();
    std::string filePath = basePath + uri;
    std::cout << "filePath: " << filePath << std::endl;

    struct stat st;
    bool isDirectory = false;
    if (stat(filePath.c_str(), &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            isDirectory = true;
            std::cout << "Confirmed: " << filePath << " is a directory" << std::endl;
        } else {
            std::cout << filePath << " exists but is not a directory" << std::endl;
        }
    } else {
        std::cerr << "stat failed for " << filePath << ": " << strerror(errno) << std::endl;
    }
    std::cout << "URI before check: " << uri << std::endl;
    std::cout << "Root: " << basePath << std::endl;
    if (!uri.empty() && uri.back() == '/') { // If the URI ends with '/', it's a directory request.
        std::cout << "Detected directory request for URI: " << uri << std::endl;
        // std::cout << "isDirectory: " << isDirectory << ", autoIndex: " << req.getAutoIndex() << std::endl;
        std::cout << "Auto index bool: " << req.getIndexLoc(basePath) << std::endl;
        if (isDirectory && req.getIndexLoc(basePath) == true) {
            std::string listing = generateDirectoryListing(filePath, uri);
            std::cout << "Directory listing result: '" << listing << "' (size: " << listing.size() << ")" << std::endl;
            if (!listing.empty()) {
                std::ostringstream responseStream;
                responseStream << "HTTP/1.1 200 OK\r\n"
                            << "Content-Length: " << listing.size() << "\r\n"
                            << "Content-Type: text/html\r\n"
                            << "\r\n"
                            << listing;
                std::cout << "Returning directory listing response" << std::endl;
                return responseStream.str();
            } else {
                std::cout << "Directory listing empty, returning 500" << std::endl;
                return ErrorHandler::getInstance().getErrorPage(500);
            }
        } else {
            filePath += "index.html";
            std::cout << "Autoindex off, updated filePath: " << filePath << std::endl;
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
    std::cout << RB << "mPOST" << RES << std::endl;
    std::string body = req.getBody();
    if (body.empty())
        return "HTTP/1.1 Error: Empty body" + ErrorHandler::getInstance().getErrorPage(500);
    // For a text upload, if body is just "text_data=" then it's empty.
    if (body.empty() || (body.find("text_data=") == 0 && body.size() == std::string("text_data=").size())) {
        // Load folder.html template to display error message at the top.
        std::ifstream file("www/templates/folder.html");
        if (!file.is_open())
            return ErrorHandler::getInstance().getErrorPage(500);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string htmlContent = buffer.str();
        file.close();

        // Replace placeholder for error message (ensure folder.html contains {{error_message}})
        replaceAll(htmlContent, "{{error_message}}", "<p style='color:red;'>Upload failed! Add something to upload.</p>");
        std::ostringstream response;
        response << "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n" << htmlContent;
        return response.str();
    }

    UploadHandler uploadHandler;
    std::string uploadedFilePath = uploadHandler.uploadReturnPath(req);
    if (uploadedFilePath.find("HTTP/1.1") == 0)
        return uploadedFilePath;

    // Check for duplicate names.
    if (!req.getFileName().empty()) {
        // Assume uploads are stored in "./www/uploads/"
        std::string uploadDir = "./www/uploads/";
        // Extract the current file name from the uploaded file path
        size_t pos = uploadedFilePath.find_last_of("/");
        std::string fileName = (pos == std::string::npos) ? uploadedFilePath : uploadedFilePath.substr(pos + 1);
        std::string fullPath = uploadDir + fileName;
        std::ifstream checkFile(fullPath.c_str());
/*         if (access(fullPath.c_str(), F_OK) == 0) { // file exists
            std::string baseName = fileName.substr(0, fileName.find_last_of('.'));
            std::string extension = fileName.substr(fileName.find_last_of('.'));
            int counter = 1;
            std::string newFileName, newFullPath;
            do {
                newFileName = "new_" + baseName + "_" + std::to_string(counter) + extension;
                newFullPath = uploadDir + newFileName;
                counter++;
            } while (access(newFullPath.c_str(), F_OK) == 0);

            if (rename(uploadedFilePath.c_str(), newFullPath.c_str()) == 0) {
                uploadedFilePath = newFullPath;
            } else {
                std::cerr << "Failed to rename file" << std::endl;
                return ErrorHandler::getInstance().getErrorPage(500);
            }
        } */
        if (checkFile.good()) {
            checkFile.close();
            std::string newFileName = "new_" + fileName;
            std::string newFullPath = uploadDir + newFileName;
            // Rename the file on disk
            if (rename(uploadedFilePath.c_str(), newFullPath.c_str()) == 0) {
                uploadedFilePath = newFullPath;
            } else {
                std::cerr << "Failed to rename file" << std::endl;
                return ErrorHandler::getInstance().getErrorPage(500);
            }
        }
    }

    // Load the upload-success.html template.
    std::ifstream file("www/templates/upload_success.html");
    if (!file.is_open())
        return ErrorHandler::getInstance().getErrorPage(500);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string htmlContent = buffer.str();
    file.close();

    // Replace the placeholder with the uploaded file path.
    replaceAll(htmlContent, "{{file_path}}", uploadedFilePath);

    std::ostringstream uploadResponse;
    uploadResponse << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << htmlContent;
    return uploadResponse.str();
}

std::string Methods::mDelete(HttpRequest &req) {
    std::cout << RB << "mDELETE" << RES << std::endl;

    req.display();

    // Instead of using the whole URI, extract the 'file' query parameter.
    // std::string fileParam = req.getUriQuery("file"); // Implement getQueryParameter() in HttpRequest
    std::map<std::string, std::string> queryMap = req.getUriQuery();

    std::string fileParam;
    for (std::map<std::string, std::string>::const_iterator it = queryMap.begin(); it != queryMap.end(); ++it) {
        if (it->first == "file") {
            fileParam = it->second;
            break;
        }
    }
    if (fileParam.empty()) {
    // File parameter missing; handle error
        return ErrorHandler::getInstance().getErrorPage(400);
    }

    // Now use fileParam for processing the delete request
    std::string basePath = "www/uploads/"; // Assuming uploads are stored here.
    std::string filePath = basePath + fileParam;
        // std::string filePath = basePath + fileName;

    if (fileParam.empty()) {
        return ErrorHandler::getInstance().getErrorPage(400);
    }

    std::cout << "File to delete: " << filePath << std::endl;

    // Verify that the filePath is indeed in the expected uploads directory.
    if (filePath.find("/uploads/") == std::string::npos) {
        std::cout << RB << "Incorrect folder" << RES << std::endl;
        return "HTTP/1.1 403\r\n\r\n" + ErrorHandler::getInstance().getErrorPage(403);
    }

    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) { // Check if the file exists
        std::cout << RB << "File doesn't exist" << RES << std::endl;
        return "HTTP/1.1 404\r\n\r\n" + ErrorHandler::getInstance().getErrorPage(404);
    }

    if (remove(filePath.c_str()) != 0) { // Try delete
        std::cout << RB << "Failed to delete" << RES << std::endl;
        return "HTTP/1.1 500\r\n\r\n" + ErrorHandler::getInstance().getErrorPage(500);
    }

    std::ostringstream deleteResponse;
    deleteResponse << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                   << "<html><body><h1>Delete Successful</h1>"
                   << "<p>The file has been deleted successfully.</p></body></html>";
    return deleteResponse.str();
}
