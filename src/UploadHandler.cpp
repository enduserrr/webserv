/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 10:34:15 by asalo             #+#    #+#             */
/*   Updated: 2025/02/24 11:20:01 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/UploadHandler.hpp"
#include "../incs/Types.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/stat.h>


UploadHandler::UploadHandler() {
    /* Create "uploads" dir with rwxr-xr-x permissions */
    mkdir("./uploads", 0755);
}

UploadHandler::~UploadHandler() {}

/* std::string extractFilenameFromMultipart(const std::string &body, const std::string &boundary) {
    std::string filename;
    std::string boundaryDelimiter = "--" + boundary;
    size_t pos = body.find(boundaryDelimiter);

    if (pos != std::string::npos) {
        size_t filenamePos = body.find("filename=\"", pos);
        if (filenamePos != std::string::npos) {
            filenamePos += 10; // Move past "filename=\""
            size_t endPos = body.find("\"", filenamePos);
            if (endPos != std::string::npos) {
                filename = body.substr(filenamePos, endPos - filenamePos);
            }
        }
    }
    return filename;
} */

std::string extractFileContentFromMultipart(const std::string &body, const std::string &boundary) {
    std::string boundaryDelimiter = "--" + boundary;
    size_t start = body.find("\r\n\r\n"); // Find start of file content
    if (start != std::string::npos) {
        start += 4; // Move past header section
        size_t end = body.find(boundaryDelimiter, start);
        if (end != std::string::npos) {
            return body.substr(start, end - start - 2); // Remove trailing CRLF
        }
    }
    return "";
}

std::string UploadHandler::uploadReturnPath(HttpRequest &req) {
    Types types; // Instance of Types class for type validation
    std::string body = req.getBody();
    std::string contentType = req.getHeader("Content-Type");

    if (contentType.empty()) {
        return ErrorHandler::getInstance().getErrorPage(415);
    }

    if (!types.isValidContent(contentType)) {
        return ErrorHandler::getInstance().getErrorPage(415);
    }

    std::string filePath;

    // Handle application/x-www-form-urlencoded (store as text file)
    if (contentType == "application/x-www-form-urlencoded") {
        filePath = "./www/uploads/upload_" + std::to_string(std::time(nullptr)) + ".txt";
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        if (!ofs) {
            return ErrorHandler::getInstance().getErrorPage(500);
        }
        ofs.write(body.c_str(), body.size());
        ofs.close();
        return filePath;
    }

    // Handle multipart/form-data
    if (contentType.find("multipart/form-data") == 0) {
        std::string boundary = contentType.substr(contentType.find("boundary=") + 9);
        if (boundary.empty()) {
            std::cout << "Multipart boundary missing" << std::endl;
            return ErrorHandler::getInstance().getErrorPage(400);
        }
        // std::string filename = extractFilenameFromMultipart(body, boundary);
        std::string filename = req.getFileName();
        if (filename.empty()) {
            std::cout << "Filename extraction failed" << std::endl;
            return ErrorHandler::getInstance().getErrorPage(400);
        }

        // Determine file type and check MIME type
        std::string extension = filename.substr(filename.find_last_of("."));
        if (!types.isValidMime(extension)) {
            std::cout << "Invalid file type: " << extension << std::endl;
            return ErrorHandler::getInstance().getErrorPage(415);
        }

        // Save the extracted file
        filePath = "./www/uploads/" + filename;
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        if (!ofs) {
            std::cout << "Failed to write uploaded file" << std::endl;
            return ErrorHandler::getInstance().getErrorPage(500);
        }
        std::string fileContent = extractFileContentFromMultipart(body, boundary);
        ofs.write(fileContent.c_str(), fileContent.size());
        ofs.close();
    }
    return filePath;
}

