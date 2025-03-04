/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:38:38 by asalo             #+#    #+#             */
/*   Updated: 2025/03/04 08:45:38 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Methods.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat
#include <unistd.h> //Access
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
    std::cout << RB << "Trying to generate dir listing" << RES << std::endl;
    std::ifstream templateFile("www/templates/listing.html"); // Use the listing template from www/templates
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

        // std::string fileItem = "<li>"
        //                        + std::string("<a href=\"") + filePath + "\">" + name + "</a>"
        //                        + " <a href='/delete?file=" + name + "' class='delete-btn'>Delete</a>"
        //                        + "</li>\n";

        // std::string fileItem = "<li>"
        //                + std::string("<input type='checkbox' name='delete_files[]' value='") + name + "'> "
        //                + "<a href=\"" + filePath + "\">" + name + "</a>"
        //                + " <a href='/delete?file=" + name + "' class='delete-btn'>Delete</a>"
        //                + "</li>\n";
        // itemsStream << fileItem;

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
    // Use the root specified in the request (populated from ServerBlock or Location)
    std::string basePath = req.getRoot();
    std::string filePath = basePath + uri;

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

/*std::string Methods::mPost(HttpRequest &req) {
    // std::cout << RB << "mPOST" << RES << std::endl;
    std::cout << "mPOST" << std::endl;
    std::string body = req.getBody();

    // Handle empty upload case
    if (body.empty() || (body.find("text_data=") == 0 && body.size() == std::string("text_data=").size())) {
        return ErrorHandler::getInstance().getErrorPage(400);
    }

    UploadHandler uploadHandler;
    std::string uploadedFilePath = uploadHandler.uploadReturnPath(req);
    if (uploadedFilePath.find("HTTP/1.1") == 0)
        return uploadedFilePath;

    // **Check File Size Before Reading**
    const size_t MAX_FILE_SIZE = 10 * 1024 * 1024; // Example: 10MB limit
    std::ifstream file(uploadedFilePath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        return ErrorHandler::getInstance().getErrorPage(500);

    size_t fileSize = file.tellg(); // Get file size
    if (fileSize > MAX_FILE_SIZE) {
        file.close();
        std::cout << "File too large: " << fileSize << " bytes" << std::endl;
        remove(uploadedFilePath.c_str()); // Delete the oversized file
        return ErrorHandler::getInstance().getErrorPage(413); // 413 Payload Too Large
    }
    file.seekg(0, std::ios::beg); // Reset read position

    // Read file contents
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    std::string htmlContent = buffer.str();

    // Load success template
    std::ifstream successFile("www/templates/upload_success.html");
    if (!successFile.is_open())
        return ErrorHandler::getInstance().getErrorPage(500);

    std::stringstream successBuffer;
    successBuffer << successFile.rdbuf();
    std::string successHtml = successBuffer.str();
    successFile.close();

    // Replace placeholders
    replaceAll(successHtml, "{{file_path}}", uploadedFilePath);

    std::ostringstream uploadResponse;
    uploadResponse << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << successHtml;
    return uploadResponse.str();
}*/


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
    std::cout << RB << "Trying to DELETE" << RES << std::endl;

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
