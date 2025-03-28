/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:38:38 by asalo             #+#    #+#             */
/*   Updated: 2025/03/28 11:46:18 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Methods.hpp"

Methods::Methods() {}

Methods::~Methods() {}

static void replaceAll(std::string &str, const std::string &from, const std::string &to) {
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
}

std::string Methods::generateDirectoryListing(const std::string &directoryPath, const std::string &uri) {
    std::ifstream templateFile("www/listing.html");

    // ↓↓↓ FALLBACK FILE IF TEMPLATE'S MISSING ↓↓↓
    if (!templateFile) {
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

    // ↓↓↓ GENERATE DIR LISTING FROM THE DIR CONTENTS ↓↓↓
    std::ostringstream itemsStream;
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

        // ↓↓↓ ADD DELETE BUTTON TO DIRECTORY LISTING RESPONSE ↓↓↓
        std::string fileItem = "<li>"
                       + std::string("<a href=\"") + filePath + "\">" + name + "</a>"
                       + " <button class='delete' data-target='/delete?file=" + name + "' data-method='DELETE'>Delete</button>"
                       + "</li>\n";
        itemsStream << fileItem;
    }
    closedir(dir);

    std::string itemsHtml = itemsStream.str();
    std::string title = "Index of " + uri;
    replaceAll(templateHtml, "{{title}}", title);
    replaceAll(templateHtml, "{{items}}", itemsHtml);

    return templateHtml;
}

std::string Methods::mGet(HttpRequest &req) {
    std::string uri = req.getUri();
    if (uri.length() >= 4 && uri.substr(uri.length() - 4) == ".ico") {
        std::string newUri = req.getHeader("Referer");
        if (newUri.find("/uploads/") != std::string::npos) {
            uri = "/uploads/";
        }
    }

    std::string basePath = req.getRoot();
    std::string filePath = basePath + uri;
    struct stat st;
    bool isDirectory = false;

    // ↓↓↓ CHECK IF IT'S A DIRECTORY ↓↓↓
    if (stat(filePath.c_str(), &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            isDirectory = true;
        } else {
            std::cout << GC << filePath << " exists but is not a directory" << RES << std::endl;
        }
    } else {
        std::cerr << RED << "stat failed for " << filePath << ": " << strerror(errno) << RES << std::endl;
    }
    std::cout << GC << "URI before check: " << uri << "\n" << "Root: " << basePath << RES << std::endl;

    // ↓↓↓ DIRECTORY REQUEST ↓↓↓
    if (!uri.empty() && uri.back() == '/') {
        if (isDirectory && req.getAutoIndex() == true && uri.length() >= 2) {
            std::string listing = generateDirectoryListing(filePath, uri);
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
                return "HTTP/1.1 500 Internal Server Error\r\n" + ErrorHandler::getInstance().getErrorPage(500);
            }
        } else { // If request is only "/" return index.html
            std::cout << filePath << std::endl; 
            filePath += "index.html"; //E: changed "="" to be "+=" root + index.html --> now works with localhost:8080 
            std::cout << "Autoindex off, updated filePath: " << filePath << std::endl;
        }
    }
    if (stat(filePath.c_str(), &st) != 0)// Check if the file exists
        return "HTTP/1.1 404 Internal Server Error\r\n" + ErrorHandler::getInstance().getErrorPage(404);

    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);// Open file for reading in binary
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Make a map of files to fetch the location from
    if (!file)
        return "HTTP/1.1 500 Internal Server Error\r\n" + ErrorHandler::getInstance().getErrorPage(500);
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileContent = ss.str();
    file.close(); // close the fstream

    std::ostringstream responseStream;// Build response
    Types types; // Create instance (consider making this a member or singleton if performance matters)

    // ↓↓↓ Use filePath to determine the MIME type ↓↓↓
    std::string mimeType = types.getMimeType(filePath);

    std::cout << GC << "Sent to MIME Check: " << filePath << std::endl;
    std::cout << "Type Result: " << mimeType << RES << std::endl;

    responseStream << "HTTP/1.1 200 OK\r\n"
                   << "Content-Length: " << fileContent.size() << "\r\n"
                   << "Content-Type: " << mimeType << "\r\n" // Use the determined type
                   << "\r\n"
                   << fileContent;
    return responseStream.str();
}

std::string Methods::mPost(HttpRequest &req) {
    std::string body = req.getBody();
    if (body.empty())
        return "HTTP/1.1 500 Internal Server Error\r\n" + ErrorHandler::getInstance().getErrorPage(500);

    // ↓↓↓ CHECK FOR EMPTY OR INVAL UPLOAD ↓↓↓
    if (body.find("text_data=") == 0 && body.size() == std::string("text_data=").size()) {
        std::ifstream file("www/listing.html");
        if (!file.is_open())
            return ErrorHandler::getInstance().getErrorPage(500);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string htmlContent = buffer.str();
        file.close();

        replaceAll(htmlContent, "{{error_message}}", "<p style='color:red;'>Upload failed! Add something to upload.</p>"); //Does this work??
        std::ostringstream response;
        response << "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n" << htmlContent;
        return response.str();
    }

    UploadHandler uploadHandler;
    std::string uploadedFilePath = uploadHandler.uploadReturnPath(req);
    if (uploadedFilePath.find("HTTP/1.1") == 0)
        return uploadedFilePath;

    // ↓↓↓ CHECK FOR DUP FILE NAMES ↓↓↓
    if (!req.getFileName().empty()) {
        std::string uploadDir = "./www/uploads/";
        std::string fileName = req.getFileName();
        std::string fullPath = uploadDir + fileName;
        bool fileExists = false;
        DIR* dir = opendir(uploadDir.c_str());
        if (!dir) {
            std::cerr << RED << "Failed to open upload directory " << uploadDir << ": " << strerror(errno) << RES << std::endl;
            return ErrorHandler::getInstance().getErrorPage(500);
        }
        struct dirent* entry;
        // // ↓↓↓ SKIP IF STARTS WITH '.' & '..' ↓↓↓
        while ((entry = readdir(dir)) != nullptr) {
            std::string entryName = entry->d_name;
            if (entryName == "." || entryName == "..") {
                continue ;
            }
            if (entryName == fileName) {
                fileExists = true;
                break ;
            }
        }
        closedir(dir);

        // ↓↓↓ CHANGE NAME IF DUP NAME EXISTS ↓↓↓
        if (fileExists) {
            std::string baseName = fileName.substr(0, fileName.find_last_of('.'));
            std::string extension = fileName.substr(fileName.find_last_of('.'));
            int counter = 1;
            std::string newFileName, newFullPath;
            do { // UPDATE TO NOT USE 'DO'!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                newFileName = baseName + "_" + std::to_string(counter) + extension;
                newFullPath = uploadDir + newFileName;
                // ↓↓↓ CHECK IF NEW NAME HAS DUPS ↓↓↓
                fileExists = false;
                dir = opendir(uploadDir.c_str());
                if (!dir) {
                    std::cerr << "Failed to reopen upload directory: " << strerror(errno) << std::endl;
                    return ErrorHandler::getInstance().getErrorPage(500);
                }
                while ((entry = readdir(dir)) != nullptr) {
                    if (std::string(entry->d_name) == newFileName) {
                        fileExists = true;
                        break;
                    }
                }
                closedir(dir);
                counter++;
            } while (fileExists);

            if (rename(uploadedFilePath.c_str(), newFullPath.c_str()) != 0) {
                std::cerr << "Failed to rename file from " << uploadedFilePath << " to " << newFullPath << ": " << strerror(errno) << std::endl;
                return ErrorHandler::getInstance().getErrorPage(500);
            }
            uploadedFilePath = newFullPath;
        }
    }

    std::ifstream file("www/upload_success.html");
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
    std::map<std::string, std::string> queryMap = req.getUriQuery();
    std::string fileParam;
    for (std::map<std::string, std::string>::const_iterator it = queryMap.begin(); it != queryMap.end(); ++it) {
        if (it->first == "file") {
            fileParam = it->second;
            break;
        }
    }
    if (fileParam.empty()) {// File parameter missing; handle error
        return ErrorHandler::getInstance().getErrorPage(400);
    }

    std::string basePath = "www/uploads/";
    std::string filePath = basePath + fileParam;

    if (fileParam.empty()) {
        return ErrorHandler::getInstance().getErrorPage(400);
    }

    std::cout << GC "File to delete: " << filePath << RES << std::endl;

    //space is replaced as "%20" in request --> replace %20 for space
    size_t pos = filePath.find("%20");
    while (pos != std::string::npos) {
        filePath.replace(pos, 3, " ");
        pos = filePath.find("%20", pos + 1);
    }

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
