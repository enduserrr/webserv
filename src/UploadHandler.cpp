/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 10:34:15 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:35:04 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "UploadHandler.hpp"
#include "Types.hpp"


UploadHandler::UploadHandler() {}

UploadHandler::~UploadHandler() {}

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
    std::cout << "uploadReturnPath" << std::endl;
    Types types; // Instance of Types class for type validation
    std::string body = req.getBody();
    std::string contentType = req.getHeader("Content-Type");
    std::string filename = req.getFileName();

    if (contentType.empty()) {
        return "HTTP/1.1 415 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", "Bad request: Empty content type (uploadHandler).", 415);
    }

    if (!types.isValidContent(contentType)) {
        return "HTTP/1.1 415 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", "Bad request: Invalid content type (uploadHandler).", 415);
    }

    std::string filePath;

    // Handle application/x-www-form-urlencoded (store as text file)
    if (contentType == "application/x-www-form-urlencoded") {
        if (filename.empty())
            filePath = "./www/uploads/upload_" + std::to_string(std::time(nullptr)) + ".txt";
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        if (!ofs) {
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", "Fail reading the file.", 500);
        }
        ofs.write(body.c_str(), body.size());
        ofs.close();
        return filePath;
    }

    // Handle multipart/form-data
    if (contentType.find("multipart/form-data") == 0) {
        std::string boundary = contentType.substr(contentType.find("boundary=") + 9);
        if (boundary.empty()) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", "Bad request (uploadHandler).", 400);
        }
        std::string filename = req.getFileName();
        if (filename.empty()) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", "Bad request (uploadHandler).", 400);
        }

        // Determine file type and check MIME type
        std::string extension = filename.substr(filename.find_last_of("."));
        if (!types.isValidMime(extension)) {
            return "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", "Unsupported media type (uploadHandler).", 415);
        }

        // Save the extracted file
        filePath = "./www/uploads/" + filename;
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        if (!ofs) {
            std::cout << "Error 500 B" << std::endl;
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", "Internal server error (uploadHandler).", 500);
        }

        std::string fileContent = extractFileContentFromMultipart(body, boundary);
        ofs.write(fileContent.c_str(), fileContent.size());
        ofs.close();
    }
    return filePath;
}

