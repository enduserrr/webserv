/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:38:38 by asalo             #+#    #+#             */
/*   Updated: 2025/05/06 10:11:49 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Methods.hpp"

/**
 * @brief   Replaces all occurrences of a substr in a str with another substr.
 */
static void replaceAll(std::string &str, const std::string &from, const std::string &to) {
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos) {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
}

/**
 * @brief   Creates an HTML directory listing page, using a template or fallback.
 *          (Del buttons for testing purposes)
 */
std::string Methods::generateDirectoryListing(const std::string &directoryPath, const std::string &uri) {
    std::ifstream templateFile("www/files.html");

    // ↓↓↓ FALLBACK FILE IF TEMPLATE'S MISSING ↓↓↓
    if (!templateFile) {
        Logger::getInstance().logLevel("INFO", "Dir listing template file not found, trying to use fallback", 0);
        std::ostringstream fallback;
        fallback << "<html><head><title>" << uri << "</title></head><body>"
                 << "<h1>Index of " << uri << "</h1><ul>";
        DIR *dir = opendir(directoryPath.c_str());
        if (!dir)
            return "";
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
    if (!dir)
        return "";
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
    std::string title = "Directory: " + uri;
    replaceAll(templateHtml, "{{title}}", title);
    replaceAll(templateHtml, "{{items}}", itemsHtml);

    return templateHtml;
}

/**
 * @brief   Process GET requests by serving files or a dir listing of a directory
 *          based on requested resource and configuration settings. (Or error page)
 */
std::string Methods::mGet(HttpRequest &req) {
    std::string uri = req.getUri();
    if (uri.length() >= 4 && uri.substr(uri.length() - 4) == ".ico") {
        std::string newUri = req.getHeader("Referer");
        std::string upload_store = req.getLocation().getUploadStore();
        if (newUri.find(upload_store) != std::string::npos)
            uri = upload_store;
    }

    std::string basePath = req.getLocation().getRoot();
    std::string filePath = basePath + uri;
    struct stat st;
    bool isDirectory = false;
    
    // ↓↓↓ CHECK IF IT'S A DIRECTORY ↓↓↓
    if (stat(filePath.c_str(), &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            isDirectory = true;
            if (isDirectory && uri.back() != '/')
                return BAD_REQ + Logger::getInstance().logLevel("ERROR", "Bad Request", 400);
        } else {
            std::ostringstream logStream;
            logStream << "Filepath: " << filePath << " exists (not a dir)";
            Logger::getInstance().logLevel("INFO", logStream.str(), 0);
        }
    } else {
        std::ostringstream logStream;
        logStream << "Stat failed on: " << filePath;
        Logger::getInstance().logLevel("ERROR", logStream.str(), 1);
    }
    std::ostringstream logStream;
    logStream << "URI before check: " << uri << ", Root: " << basePath;
    Logger::getInstance().logLevel("INFO", logStream.str(), 0);

    // ↓↓↓ DIRECTORY REQUEST ↓↓↓
    if (!uri.empty() && uri.back() == '/') {
        std::string locIndex = req.getLocation().getIndex();
        if (!locIndex.empty() && uri.length() > 1) {
            filePath = basePath + "/" + locIndex;
            Logger::getInstance().logLevel("INFO", "Returning predefined index page", 0);
        } else if (isDirectory && req.getLocation().getAutoIndex() == true && uri.length() >= 2) {
            std::string listing = generateDirectoryListing(filePath, uri);
            if (!listing.empty()) {
                std::ostringstream responseStream;
                responseStream << "HTTP/1.1 200 OK\r\n"
                            << "Content-Length: " << listing.size() << "\r\n"
                            << "Content-Type: text/html\r\n"
                            << "\r\n"
                            << listing;
                Logger::getInstance().logLevel("INFO", "Returning directory listing", 0);
                return responseStream.str();
            } else {
                return INTERNAL + Logger::getInstance().logLevel("ERROR", "Directory listing is empty", 500);
            }
        } else { // If request is only "/" return index.html
            filePath += "index.html";
        }
    }
    if (stat(filePath.c_str(), &st) != 0)// Check if the file exists
        return NOT_FOUND + Logger::getInstance().logLevel("ERROR", "mGET: stat() failed", 404);

    std::ifstream file(filePath.c_str(), std::ios::in | std::ios::binary);// Open file for reading in binary
    if (!file)
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "mGet: failed to read file.", 500);
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileContent = ss.str();
    file.close();

    std::string mimeType = Types::getInstance().getMimeType(filePath);
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n"
                   << "Content-Length: " << fileContent.size() << "\r\n"
                   << "Content-Type: " << mimeType << "\r\n"
                   << "\r\n"
                   << fileContent;
    return responseStream.str();
}

/**
 * @brief   Handles POST requests, manages file uploads via UploadHandler,
 *          checks for and resolves duplicate filenames, and returns a success page.
 */
std::string Methods::mPost(HttpRequest &req) {
    std::string body = req.getBody();
    if (body.empty())
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Empty request body.", 500);

    // // ↓↓↓ CHECK FOR EMPTY ↓↓↓
    if (body.find("text=") == 0 && body.size() == std::string("text=").size()) //changed from text_data=
        return BAD_REQ + Logger::getInstance().logLevel("ERROR", "POST request with an empty body", 400);

    UploadHandler uploadHandler;
    std::string uploadedFilePath = uploadHandler.uploadReturnPath(req);
    if (uploadedFilePath.find("HTTP/1.1") == 0)
        return uploadedFilePath;

    // ↓↓↓ CHECK FOR DUP FILE NAMES ↓↓↓
    if (!req.getFileName().empty()) {
        std::string uploadDir = req.getLocation().getRoot() + req.getLocation().getUploadStore(); //Server side upload location
        std::string fileName = req.getFileName();
        std::string fullPath = uploadDir + fileName;
        bool fileExists = false;
        DIR* dir = opendir(uploadDir.c_str());
        if (!dir)
            return INTERNAL + Logger::getInstance().logLevel("ERROR", "mPOST: failed to open directory.", 500);
        struct dirent* entry;
        // ↓↓↓ SKIP IF STARTS WITH '.' & '..' ↓↓↓
        while ((entry = readdir(dir)) != nullptr) {
            std::string entryName = entry->d_name;
            if (entryName == "." || entryName == "..")
                continue ;
            if (entryName == fileName) {
                fileExists = true;
                break ;
            }
        }
        closedir(dir);

        if (fileExists) {
            std::string baseName = fileName.substr(0, fileName.find_last_of('.'));
            std::string extension = fileName.substr(fileName.find_last_of('.'));
            int counter = 1;
            std::string newFileName, newFullPath;

            // Initialize the first filename to check
            newFileName = baseName + "_" + std::to_string(counter) + extension;
            newFullPath = uploadDir + newFileName;

            // Loop until a unique filename is found
            while (fileExists) {
                // Check if new name has duplicates
                fileExists = false;
                dir = opendir(uploadDir.c_str());
                if (!dir)
                    return INTERNAL + Logger::getInstance().logLevel("ERROR", "Failed to reopen directory", 500);
                while ((entry = readdir(dir)) != nullptr) {
                    if (std::string(entry->d_name) == newFileName) {
                        fileExists = true;
                        break;
                    }
                }
                closedir(dir);

                // If the name exists, increment counter and generate a new filename
                if (fileExists) {
                    counter++;
                    newFileName = baseName + "_" + std::to_string(counter) + extension;
                    newFullPath = uploadDir + newFileName;
                }
            }
            if (rename(uploadedFilePath.c_str(), newFullPath.c_str()) != 0)
                return INTERNAL + Logger::getInstance().logLevel("ERROR", "Failed to rename file.", 500);
            uploadedFilePath = newFullPath;
        }
    }

    std::ifstream file("www/upload_success.html");
    if (!file.is_open())
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Failed to upload file.", 500);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string htmlContent = buffer.str();
    file.close();

    replaceAll(htmlContent, "{{file_path}}", uploadedFilePath);

    std::ostringstream uploadResponse;
    uploadResponse << "HTTP/1.1 200 OK\r\n"
                << "Content-Length: " << htmlContent.size() << "\r\n"
                << "Content-Type: text/html\r\n"
                << "Connection: keep-alive\r\n"
                << "\r\n"
                << htmlContent;
    return uploadResponse.str();
}

/**
 * @brief   Processes DELETE requests, extracts filename from query,
 *          validates path, removes the file, and returns a response.
 */
std::string Methods::mDelete(HttpRequest &req) {
    std::map<std::string, std::string> queryMap = req.getUriQuery();
    std::string fileParam;
    for (std::map<std::string, std::string>::const_iterator it = queryMap.begin(); it != queryMap.end(); ++it) {
        if (it->first == "file") {
            fileParam = it->second;
            break;
        }
    }
    if (fileParam.empty())
        return BAD_REQ + Logger::getInstance().logLevel("ERROR", "mDELETE: missing file parameter.", 400);

    std::string basePath = req.getRoot() + req.getLocation().getUploadStore();
    std::string filePath = basePath + fileParam;

    if (fileParam.empty())
        return BAD_REQ + Logger::getInstance().logLevel("ERROR", "mDELETE: empty file parameter", 400);

    std::ostringstream logStream;
    logStream << "File to be deleted: " << filePath;
    Logger::getInstance().logLevel("INFO", logStream.str(), 0);

    //space is replaced as "%20" in request --> replace %20 for space
    size_t pos = filePath.find("%20");
    while (pos != std::string::npos) {
        filePath.replace(pos, 3, " ");
        pos = filePath.find("%20", pos + 1);
    }

    if (filePath.find(basePath) == std::string::npos)
        return FORBIDDEN + Logger::getInstance().logLevel("ERROR", "Incorrect folder.", 403);

    struct stat st;
    if (stat(filePath.c_str(), &st) != 0) // Check if the file exists
        return NOT_FOUND + Logger::getInstance().logLevel("ERROR", "File does not exist.", 404);

    if (remove(filePath.c_str()) != 0) // Try delete
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Failed to delete file.", 500);

    std::ostringstream deleteResponse;
    deleteResponse << "HTTP/1.1 200 OK\r\n"
                << "Content-Length: 0\r\n"
                << "Content-Type: text/html\r\n"
                << "\r\n";
    return deleteResponse.str();
}
