/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UploadHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/03 10:34:15 by asalo             #+#    #+#             */
/*   Updated: 2025/02/04 11:13:00 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/UploadHandler.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <sys/stat.h>


UploadHandler::UploadHandler() {
    /* Create "uploads" dir with rwxr-xr-x permissions */
    mkdir("./uploads", 0755);
}

UploadHandler::~UploadHandler() {}

std::string UploadHandler::upload(HttpRequest &req) {
    /* Get raw data to be uploaded*/
    std::string body = req.getBody();

    /* Generate file name using creation time */
    std::time_t now = std::time(nullptr);
    std::ostringstream filename;
    filename << "./uploads/upload_" << now << ".dat";

    /* Attempt to write the uploaded content to the file */
    std::ofstream ofs(filename.str().c_str(), std::ios::binary);
    if (!ofs) {
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               + ErrorHandler::getInstance().getErrorPage(500);
    }
    ofs.write(body.c_str(), body.size());
    ofs.close();

    /* Success response */
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                   << "<html><body><h1>Upload Successful</h1>"
                   << "<p>Your file has been saved as " << filename.str() << ".</p>"
                   << "</body></html>";

    return responseStream.str();
}

std::string UploadHandler::uploadReturnPath(HttpRequest &req) {
    std::string body = req.getBody();
    /* Generate file name using creation time */
    std::time_t now = std::time(nullptr);
    std::ostringstream filename;
    filename << "./uploads/upload_" << now << ".dat";// Why .dat?
    /* Get the generated filename as a string */
    std::string filePath = filename.str();

    /* Attempt to write the uploaded content to the file */
    std::ofstream ofs(filePath.c_str(), std::ios::binary);
    if (!ofs) {//Update ofs name to ofsContent or fdIn
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               + ErrorHandler::getInstance().getErrorPage(500);
    }
    ofs.write(body.c_str(), body.size());
    ofs.close();

    return filePath;
}
