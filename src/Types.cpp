/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Types.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:19:04 by asalo             #+#    #+#             */
/*   Updated: 2025/02/20 11:06:24 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Types.hpp"

/* Init MIME types map */
Types::Types() {
    _mimeTypes = {
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
        _contentTypes = {
        "application/x-www-form-urlencoded",
        "multipart/form-data"
    };
}

Types::~Types() {}

// Get MIME type from an extension
std::string Types::getMimeType(const std::string &extension) const {
    std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(extension);
    if (it != _mimeTypes.end())
        return it->second;
    return "application/octet-stream"; // Default type for unknown files
}

bool Types::isValidMime(const std::string &extension) const {
    return _mimeTypes.find(extension) != _mimeTypes.end();
}

bool Types::isValidContent(const std::string &contentType) const {
    for (const std::string &type : _contentTypes) {
        if (contentType.find(type) == 0) {  // Handle cases like `multipart/form-data; boundary=...`
            return true;
        }
    }
    return false;
}


