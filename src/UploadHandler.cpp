/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 10:34:15 by asalo             #+#    #+#             */
/*   Updated: 2025/02/19 13:03:36 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/UploadHandler.hpp"
#include "../incs/Mime.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/stat.h>


UploadHandler::UploadHandler() {
    /* Create "uploads" dir with rwxr-xr-x permissions */
    mkdir("./uploads", 0755);
}

UploadHandler::~UploadHandler() {}

// std::string UploadHandler::upload(HttpRequest &req) {
//     /* Get raw data to be uploaded*/
//     std::string body = req.getBody();

//     std::time_t now = std::time(nullptr);
//     std::ostringstream filename;
//     filename << "./uploads/upload_" << now << ".dat";

//     std::ofstream ofs(filename.str().c_str(), std::ios::binary);
//     if (!ofs) {
//         return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
//                + ErrorHandler::getInstance().getErrorPage(500);
//     }
//     ofs.write(body.c_str(), body.size());
//     ofs.close();

//     std::ostringstream responseStream;
//     responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
//                    << "<html><body><h1>Upload Successful</h1>"
//                    << "<p>Your file has been saved as " << filename.str() << ".</p>"
//                    << "</body></html>";

//     return responseStream.str();
// }

std::string UploadHandler::uploadReturnPath(HttpRequest &req) {
    std::string body = req.getBody();

    // Get the content type from the request header --> DOESN'T WORK NOW => INCORRECT REQUEST PARSING
    std::string contentType = req.getHeader("Content-Type");
    if (contentType.empty()) {
        std::cout << "Content-type header missing" << std::endl;
        return "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n"
               + ErrorHandler::getInstance().getErrorPage(415);
    }

    std::vector<std::pair<std::string, std::string>> acceptedTypes = { // Accepted mime types
        {".html", "text/html"},
        {".htm",  "text/html"},
        {".css",  "text/css"},
        {".ico",  "image/x-icon"},
        {".avi",  "video/x-msvideo"},
        {".bmp",  "image/bmp"},
        {".doc",  "application/msword"},
        {".gif",  "image/gif"},
        {".gz",   "application/x-gzip"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png",  "image/png"},
        {".txt",  "text/plain"},
        {".mp3",  "audio/mp3"},
        {".pdf",  "application/pdf"}
    };

    std::string extension;
    for (const auto &pair : acceptedTypes) { // Find Content-Type
        if (pair.second == contentType) {
            extension = pair.first;
            break;
        }
    }

    // If no accepted MIME type match don't upload
    if (extension.empty()) {
        std::cout << "No matching media type." << std::endl;
        return "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n"
               + ErrorHandler::getInstance().getErrorPage(415);
    }

    // Gen file name
    std::time_t now = std::time(nullptr);
    std::ostringstream filename;
    filename << "./uploads/upload_" << now << extension;
    std::string filePath = filename.str();

    // Try writing the uploaded content to the file
    std::ofstream ofs(filePath.c_str(), std::ios::binary);
    if (!ofs) {
        std::cout << "Failed to write uploaded content." << std::endl;
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               + ErrorHandler::getInstance().getErrorPage(500);
    }
    ofs.write(body.c_str(), body.size());
    ofs.close();

    return filePath;
}
