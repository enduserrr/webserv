/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 10:34:15 by asalo             #+#    #+#             */
/*   Updated: 2025/05/05 18:18:20 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "UploadHandler.hpp"
#include "Types.hpp"


UploadHandler::UploadHandler() {}

UploadHandler::~UploadHandler() {}

/**
 * @brief   Decodes a URL-encoded string (form data), handling %XX, +, and & characters.
 */
std::string decodeBnry(const std::string& binaryContent) {
    std::string decoded;
    std::string content = binaryContent;

    // Remove 'text=' from the beginning if present
    if (content.length() >= 5 && content.substr(0, 5) == "text=") {
        content = content.substr(5); // Skip past 'text='
    }

    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '%' && i + 2 < content.length()) {
            if (std::isxdigit(content[i + 1]) && std::isxdigit(content[i + 2])) {
                std::string hexStr = content.substr(i + 1, 2);
                unsigned char decodedChar = static_cast<unsigned char>(std::stoi(hexStr, nullptr, 16));
                decoded += decodedChar; // Decodes %20 to space, %0A to newline, etc.
                i += 2;
                continue;
            }
        } else if (content[i] == '&') {
            decoded += "\n"; // Replace '&' with newline for readability
            continue;
        } else if (content[i] == '+') {
            decoded += " "; // Replace '+' with space for readability
            continue;
        }
        decoded += content[i]; // Keep other characters as-is
    }
    return decoded;
}

/**
 * @brief   Handles file uploads based on content type.
 *          Decodes data if necessary, validates types, saves the file,
 *          and returns the saved file path or an error response.
 */
std::string UploadHandler::uploadReturnPath(HttpRequest &req) {
    std::string body = req.getBody();
    std::string contentType = req.getHeader("Content-Type");
    std::string filename = req.getFileName();
    if (contentType.empty())
        return UNSUPPORTED + Logger::getInstance().logLevel("ERROR", "Bad request: Empty content type (uploadHandler).", 415);
    if (Types::getInstance().isValidContent(contentType) == false)
        return UNSUPPORTED + Logger::getInstance().logLevel("ERROR", "Bad request: Invalid content type (uploadHandler).", 415);

    std::string filePath;

    if (contentType == "application/x-www-form-urlencoded") {
        if (filename.empty())
            filePath = req.getLocation().getRoot() + req.getLocation().getUploadStore() + "/upload_" + std::to_string(std::time(nullptr));
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        std::string decodedBody = decodeBnry(body);
        ofs.write(decodedBody.c_str(), decodedBody.size());
        ofs.close();
        return filePath;
    }
    else if (contentType.find("multipart/form-data") == 0) {
        std::string boundary = contentType.substr(contentType.find("boundary=") + 9);
        if (boundary.empty())
            return BAD_REQ + Logger::getInstance().logLevel("ERROR", "Bad request (uploadHandler).", 400);
        if (filename.empty())
            return BAD_REQ + Logger::getInstance().logLevel("ERROR", "Bad request (uploadHandler).", 400);

        // Determine file type and check MIME type
        std::string extension = filename.substr(filename.find_last_of("."));
        if (Types::getInstance().isValidMime(extension) == false)
            return UNSUPPORTED + Logger::getInstance().logLevel("ERROR", "Unsupported media type (uploadHandler).", 415);

        // Save the extracted file
        filePath = req.getLocation().getRoot() + req.getLocation().getUploadStore() + filename;
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        if (!ofs)
            return INTERNAL + Logger::getInstance().logLevel("ERROR", "Internal server error (uploadHandler).", 500);
        std::string fileContent = body;
        ofs.write(fileContent.c_str(), fileContent.size());
        ofs.close();
    }
    else if (contentType == "text/plain") {
        filePath = req.getLocation().getRoot() + req.getLocation().getUploadStore() + "/upload_" + std::to_string(std::time(nullptr));
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        ofs.write(body.c_str(), body.size());
        ofs.close();
    }
    return filePath;
}
