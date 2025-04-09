/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 10:34:15 by asalo             #+#    #+#             */
/*   Updated: 2025/04/09 19:15:56 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "UploadHandler.hpp"
#include "Types.hpp"


UploadHandler::UploadHandler() {}

UploadHandler::~UploadHandler() {}

std::string UploadHandler::uploadReturnPath(HttpRequest &req) {
    std::string body = req.getBody();
    std::string contentType = req.getHeader("Content-Type");
    std::string filename = req.getFileName();
    if (contentType.empty()) {
        return UNSUPPORTED + Logger::getInstance().logLevel("ERROR", "Bad request: Empty content type (uploadHandler).", 415);
    }

    if (Types::getInstance().isValidContent(contentType) == false) {
        return UNSUPPORTED + Logger::getInstance().logLevel("ERROR", "Bad request: Invalid content type (uploadHandler).", 415);
    }

    std::string filePath;

    // Handle application/x-www-form-urlencoded (store as text file)
    if (contentType == "application/x-www-form-urlencoded") {
        if (filename.empty())
            filePath = "./www/uploads/upload_" + std::to_string(std::time(nullptr)) + ".txt";
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        if (!ofs) {
            return INTERNAL + Logger::getInstance().logLevel("ERROR", "Fail reading the file.", 500);
        }
        ofs.write(body.c_str(), body.size());
        ofs.close();
        return filePath;
    }
    else if (contentType.find("multipart/form-data") == 0) {
        std::string boundary = contentType.substr(contentType.find("boundary=") + 9);
        if (boundary.empty()) {
            return BAD_REQ + Logger::getInstance().logLevel("ERROR", "Bad request (uploadHandler).", 400);
        }
        if (filename.empty()) {
            return BAD_REQ + Logger::getInstance().logLevel("ERROR", "Bad request (uploadHandler).", 400);
        }

        // Determine file type and check MIME type
        std::string extension = filename.substr(filename.find_last_of("."));
        if (Types::getInstance().isValidMime(extension) == false) {
            return UNSUPPORTED + Logger::getInstance().logLevel("ERROR", "Unsupported media type (uploadHandler).", 415);
        }

        // Save the extracted file
        filePath = "./www/uploads/" + filename;
        std::ofstream ofs(filePath.c_str(), std::ios::binary);
        if (!ofs) {
            return INTERNAL + Logger::getInstance().logLevel("ERROR", "Internal server error (uploadHandler).", 500);
        }
        std::string fileContent = body;
        ofs.write(fileContent.c_str(), fileContent.size());
        ofs.close();
    }
    return filePath;
}
