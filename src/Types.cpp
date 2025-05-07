#include "Types.hpp"

/**
 * @brief   Types class contructor that sets the valid mime and content types.
 */
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
        "multipart/form-data",
        "text/plain"
    };
}

/**
 * @brief   Singleton instance getter.
 */
Types& Types::getInstance() {
    static Types instance;
    return instance;
}

Types::~Types() {}

/**
 * @brief   Extract the mime (file) type of the given file.
 */
std::string Types::getMimeType(const std::string &filePath) const {
    size_t dotPos = filePath.rfind('.'); // pos of the last dot
    if (dotPos == std::string::npos) {
        return "application/octet-stream";
    }
    std::string ext = filePath.substr(dotPos); // Extract extension
    // Convert extension to lowercase for case-insensitive lookup
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    // lookup with the now lowercased extension
    std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(ext);
    if (it != _mimeTypes.end()) {
        return it->second;
    }
    return "application/octet-stream";
}

/**
 * @brief   Check if mime type is valid to be processed.
 *          Mime type is a standard indicating the file format.
 */
bool Types::isValidMime(const std::string &filePath) const {
    size_t dotPos = filePath.rfind('.');
    if (dotPos == std::string::npos) {
        return false;
    }

    std::string ext = filePath.substr(dotPos);
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return _mimeTypes.find(ext) != _mimeTypes.end();
}

bool Types::isValidContent(const std::string &contentType) const {
    for (const std::string &type : _contentTypes) {
        if (contentType.find(type) == 0) {  // Handle cases like `multipart/form-data; boundary=...`
            return true;
        }
    }
    return false;
}


