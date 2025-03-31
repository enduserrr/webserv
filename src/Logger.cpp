/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:21:14 by asalo             #+#    #+#             */
/*   Updated: 2025/03/31 10:43:13 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "Logger.hpp"

std::string Logger::loadFileContent(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    if (!file) {
        std::cerr << "Error: Could not open error page file: " << filePath << std::endl;
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

Logger::Logger() {
    _state = 0;
    std::string baseDir = "www/error_pages/";

    _errorPages[400] = loadFileContent(baseDir + "400.html");
    _errorPages[403] = loadFileContent(baseDir + "403.html");
    _errorPages[404] = loadFileContent(baseDir + "404.html");
    _errorPages[408] = loadFileContent(baseDir + "408.html");
    _errorPages[415] = loadFileContent(baseDir + "415.html");
    _errorPages[413] = loadFileContent(baseDir + "413.html");
    _errorPages[429] = loadFileContent(baseDir + "429.html");
    _errorPages[500] = loadFileContent(baseDir + "500.html");
    _defaultErrorPage = loadFileContent(baseDir + "default.html");

    // Fallback option if any file fails to load
    if (_errorPages[400].empty())
        _errorPages[400] = "<html><body><h1>400 Bad Request</h1><p>Your request cannot be processed.</p></body></html>";
    if (_errorPages[403].empty())
        _errorPages[403] = "<html><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";
    if (_errorPages[404].empty())
        _errorPages[404] = "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>";
    if (_errorPages[408].empty())
        _errorPages[408] = "<html><body><h1>408 Request Timeout</h1><p>The server timed out waiting for the request.</p></body></html>";
    if (_errorPages[429].empty())
        _errorPages[429] = "<html><body><h1>429 Too Many Requests</h1><p>The server can't accept more requests at this time</p></body></html>";
    if (_errorPages[500].empty())
        _errorPages[500] = "<html><body><h1>500 Internal Server Error</h1><p>An unexpected error occurred.</p></body></html>";

    if (_defaultErrorPage.empty())
        _defaultErrorPage = "<html><body><h1>Error</h1><p>An error occurred.</p></body></html>";
}

Logger &Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    // std::cout << "ErrorHandler Destructor." << std::endl;
}

std::string Logger::getCurrentTimestamp() const{
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    // Use std::gmtime for UTC or std::localtime for local time
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::logLevel(std::string level, const std::string &message, int code) {
    if (level == "INFO") {
        _state = code;
        std::cout << GREY_B << "[" << getCurrentTimestamp() << "] " << GB << "[INFO]: " << RES GC << message << RES << std::endl;
        return std::string();
    }
    else if (level == "WARNING") {
        _state = code;
        std::cout << GREY_B << "[" << getCurrentTimestamp() << "] " << YB << "[WARNING]: " << RES GC << message << RES << std::endl;
        return std::string();
    }
    else if (level == "ERROR") {
        _state = 1;
        std::cerr << GREY_B << "[" << getCurrentTimestamp() << "] " << RB << "[ERROR]: " << RES GC << message << RES << std::endl;
        if (_errorPages.find(code) != _errorPages.end()) {
            return _errorPages[code];
        }
        return _defaultErrorPage;
    }
    else if (level == "SYS_ERROR") {
        _state = code;
        std::cout << GREY_B << "[" << getCurrentTimestamp() << "] " << RB << "[ERROR]: " << RES GC << message << RES << std::endl;
        return std::string();
    }
    std::cerr << GREY_B << "[" << getCurrentTimestamp() << "] " << RB << "[ERROR]: " << RES GC << "UNKNOW ERROR" << RES << std::endl;
    return "<html><body><h1>Error</h1><p>An unknown error occurred.</p></body></html>";
}

std::string Logger::getErrorPage(int code) {
    if (_errorPages.find(code) != _errorPages.end()) {
        return _errorPages[code];
    }
    return _defaultErrorPage;
}

void Logger::setCustomErrorPage(int code, const std::string &pageContent) {
    _errorPages[code] = pageContent;
}
/* ****************************************************************************** */
/* ============================================================================== */
/* ****************************************************************************** */
/* std::string Logger::loadFileContent(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    if (!file) {
        // Use the internal log method for consistency, but log as an error
        // Note: This log happens *during* construction, before the instance might be fully ready,
        // but std::cerr is safe. We can't call log() directly here yet.
        std::cerr << "[ErrorHandler Setup]: Error: Could not open error page file: " << filePath << std::endl;
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string Logger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN"; // Should not happen
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    // Use std::gmtime for UTC or std::localtime for local time
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


// --- Constructor and Destructor Implementation ---

Logger::ErrorHandler() {
    std::cout << "ErrorHandler Constructor: Initializing..." << std::endl; // Added for clarity
    std::string baseDir = "www/error_pages/"; // Consider making this configurable

    // Define standard HTTP codes we want pages for
    const int errorCodes[] = {400, 403, 404, 405, 408, 413, 415, 429, 500, 501, 505};
    const std::map<int, std::string> defaultHtml = {
        {400, "<html><body><h1>400 Bad Request</h1><p>Your request cannot be processed.</p></body></html>"},
        {403, "<html><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>"},
        {404, "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>"},
        {405, "<html><body><h1>405 Method Not Allowed</h1><p>The method specified in the Request-Line is not allowed for the resource identified by the Request-URI.</p></body></html>"},
        {408, "<html><body><h1>408 Request Timeout</h1><p>The server timed out waiting for the request.</p></body></html>"},
        {413, "<html><body><h1>413 Payload Too Large</h1><p>The server is refusing to process a request because the request payload is larger than the server is willing or able to process.</p></body></html>"},
        {415, "<html><body><h1>415 Unsupported Media Type</h1><p>The server is refusing to service the request because the entity of the request is in a format not supported by the requested resource for the requested method.</p></body></html>"},
        {429, "<html><body><h1>429 Too Many Requests</h1><p>The server can't accept more requests at this time.</p></body></html>"},
        {500, "<html><body><h1>500 Internal Server Error</h1><p>An unexpected error occurred on the server.</p></body></html>"},
        {501, "<html><body><h1>501 Not Implemented</h1><p>The server does not support the functionality required to fulfill the request.</p></body></html>"},
        {505, "<html><body><h1>505 HTTP Version Not Supported</h1><p>The server does not support, or refuses to support, the HTTP protocol version that was used in the request message.</p></body></html>"}
    };

    // Load default page first
    _defaultErrorPage = loadFileContent(baseDir + "default.html");
    if (_defaultErrorPage.empty()) {
         std::cerr << "[ErrorHandler Setup]: Warning: Failed to load default error page. Using hardcoded default." << std::endl;
        _defaultErrorPage = "<html><body><h1>Error</h1><p>An error occurred.</p></body></html>";
    }


    // Load specific error pages
    for (int code : errorCodes) {
        std::string filePath = baseDir + std::to_string(code) + ".html";
        std::string content = loadFileContent(filePath);
        if (!content.empty()) {
            _errorPages[code] = content;
        } else {
             std::cerr << "[ErrorHandler Setup]: Warning: Failed to load page for code " << code << ". Using hardcoded default." << std::endl;
            // Use hardcoded fallback if file loading failed
             auto it = defaultHtml.find(code);
             if (it != defaultHtml.end()) {
                 _errorPages[code] = it->second;
             } else {
                 // Should not happen if defaultHtml is complete, but as a safeguard:
                  std::cerr << "[ErrorHandler Setup]: Error: No hardcoded default found for code " << code << "!" << std::endl;
                  // Use a generic message embedding the code
                 _errorPages[code] = "<html><body><h1>Error " + std::to_string(code) + "</h1><p>An error occurred.</p></body></html>";
             }
        }
    }
     std::cout << "ErrorHandler Constructor: Initialization complete." << std::endl;
}

Logger::~ErrorHandler() {
    // Destructor remains simple, members handle their own cleanup.
    std::cout << "ErrorHandler Destructor." << std::endl;
}

// --- Public Method Implementations ---

ErrorHandler &Logger::getInstance() {
    // Static local variable ensures thread-safe initialization in C++11+
    static ErrorHandler instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string &message) {
    // Output log to std::cerr (common for server logs)
    std::cerr << "[" << getCurrentTimestamp() << "] [" << logLevelToString(level) << "] "
              << message << std::endl;
}

std::string Logger::handleHttpError(int httpStatusCode, const std::string &errorMessage) {
    // 1. Log the error
    // Construct a more informative log message including the status code
    std::string logMessage = "HTTP Status " + std::to_string(httpStatusCode) + ": " + errorMessage;
    log(LogLevel::ERROR, logMessage); // Always log HTTP errors as ERROR level

    // 2. Get the corresponding error page content
    return getErrorPageContent(httpStatusCode);
}

std::string Logger::getErrorPageContent(int httpStatusCode) {
    // Find the specific page for the code
    std::map<int, std::string>::const_iterator it = _errorPages.find(httpStatusCode);
    if (it != _errorPages.end()) {
        return it->second; // Return specific page content
    }

    // If no specific page found, return the default error page
    // Optionally log a warning that the default page is being used
    log(LogLevel::WARNING, "No specific error page found for HTTP status " + std::to_string(httpStatusCode) + ". Serving default error page.");
    return _defaultErrorPage;
}

void Logger::setCustomErrorPage(int httpStatusCode, const std::string &pagePath) {
    std::string content = loadFileContent(pagePath);
    if (!content.empty()) {
        _errorPages[httpStatusCode] = content;
        log(LogLevel::INFO, "Custom error page set for HTTP status " + std::to_string(httpStatusCode) + " from path: " + pagePath);
    } else {
        log(LogLevel::WARNING, "Failed to load custom error page for HTTP status " + std::to_string(httpStatusCode) + " from path: " + pagePath + ". No change made.");
    }
}

void Logger::setCustomErrorPageContent(int httpStatusCode, const std::string &pageContent) {
     if (!pageContent.empty()) {
        _errorPages[httpStatusCode] = pageContent;
        log(LogLevel::INFO, "Custom error page content set directly for HTTP status " + std::to_string(httpStatusCode) + ".");
    } else {
        log(LogLevel::WARNING, "Attempted to set empty custom error page content for HTTP status " + std::to_string(httpStatusCode) + ". No change made.");
    }
} */
