/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Types.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 10:19:04 by asalo             #+#    #+#             */
/*   Updated: 2025/03/28 11:12:57 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/Types.hpp"
#include <algorithm>

Types::Types() {
    _mimeTypes = {
        {".html", "text/html"},
        {".htm", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".ico", "image/x-icon"},
        {".avi", "video/x-msvideo"},
        {".bmp", "image/bmp"},
        {".doc", "application/msword"},
        {".gif", "image/gif"},
        {".gz", "application/x-gzip"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".png", "image/png"},
        {".txt", "text/plain"},
        {".mp3", "audio/mp3"},
        {".pdf", "application/pdf"}
    };
        _contentTypes = {
        "application/x-www-form-urlencoded",
        "multipart/form-data"
    };
}

Types::~Types() {}

// std::string Types::getMimeType(const std::string &extension) const {
//     std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(extension);
//     if (it != _mimeTypes.end())
//         return it->second;
//     return "application/octet-stream"; // Default for unknown files
// }

std::string Types::getMimeType(const std::string &filePath) const {
    // Find the position of the last dot
    size_t dotPos = filePath.rfind('.');

    // If no dot found, return default
    if (dotPos == std::string::npos) {
        return "application/octet-stream";
    }

    // Extract the extension, including the dot
    std::string ext = filePath.substr(dotPos);

    // Convert extension to lowercase for case-insensitive lookup
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); }); // C++11 and safer

    // Perform the lookup using the extracted, lowercase extension
    std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(ext);
    if (it != _mimeTypes.end()) {
        return it->second; // Return the found MIME type
    }

    // If extension not in map, return default
    return "application/octet-stream";
}

// bool Types::isValidMime(const std::string &extension) const {
//     return _mimeTypes.find(extension) != _mimeTypes.end();
// }

bool Types::isValidMime(const std::string &filePath) const {
    size_t dotPos = filePath.rfind('.');
    if (dotPos == std::string::npos) {
        return false; // No extension, so not a valid *known* MIME type in our map
    }

    std::string ext = filePath.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    // Check if the lowercase extension exists as a key in the map
    return _mimeTypes.find(ext) != _mimeTypes.end();
}

bool Types::isValidContent(const std::string &contentType) const {
    for (const std::string &type : _contentTypes) {
        if (contentType.find(type) == 0) {  // Handle cases like `multipart/form-data; boundary=...`
            return true;
        }
    }
    std::cout << "Type is: " << contentType << std::endl;
    return false;
}


