/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:21:14 by asalo             #+#    #+#             */
/*   Updated: 2025/02/10 10:31:31 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/ErrorHandler.hpp"
#define WB  "\033[7;1;91m"
#define RES "\033[0m"

/* // Default error pages for common HTTP status codes
ErrorHandler::ErrorHandler() {
    _errorPages[400] = "<html><body><h1>400 Bad Request</h1><p>Your request cannot be processed.</p></body></html>";
    _errorPages[404] = "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>";
    _errorPages[500] = "<html><body><h1>500 Internal Server Error</h1><p>An unexpected error occurred.</p></body></html>";
    _errorPages[403] = "<html><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";
    _errorPages[408] = "<html><body><h1>408 Request Timeout</h1><p>The server timed out waiting for the request.</p></body></html>";
    _defaultErrorPage = "<html><body><h1>Error</h1><p>An unknown error occurred.</p></body></html>";
    // std::cout << "ErrorHandler Constructor." << std::endl;
} */

std::string ErrorHandler::loadFileContent(const std::string &filePath) {
    std::ifstream file(filePath.c_str());
    if (!file) {
        std::cerr << "Error: Could not open error page file: " << filePath << std::endl;
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

ErrorHandler::ErrorHandler() {
    // Load error pages from the directory "www/error_pages"
    std::string baseDir = "www/error_pages/";

    // Try to load each error page file.
    _errorPages[400] = loadFileContent(baseDir + "400.html");
    _errorPages[404] = loadFileContent(baseDir + "404.html");
    _errorPages[500] = loadFileContent(baseDir + "500.html");
    _errorPages[403] = loadFileContent(baseDir + "403.html");
    _errorPages[408] = loadFileContent(baseDir + "408.html");

    // Load a default error page.
    _defaultErrorPage = loadFileContent(baseDir + "default.html");

    // Optionally, if any file fails to load, you can assign a hard-coded fallback:
    if (_errorPages[400].empty())
        _errorPages[400] = "<html><body><h1>400 Bad Request</h1><p>Your request cannot be processed.</p></body></html>";
    if (_errorPages[404].empty())
        _errorPages[404] = "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>";
    if (_errorPages[500].empty())
        _errorPages[500] = "<html><body><h1>500 Internal Server Error</h1><p>An unexpected error occurred.</p></body></html>";
    if (_errorPages[403].empty())
        _errorPages[403] = "<html><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";
    if (_errorPages[408].empty())
        _errorPages[408] = "<html><body><h1>408 Request Timeout</h1><p>The server timed out waiting for the request.</p></body></html>";
    if (_defaultErrorPage.empty())
        _defaultErrorPage = "<html><body><h1>Error</h1><p>An unknown error occurred.</p></body></html>";
}

ErrorHandler &ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::~ErrorHandler() {
    std::cout << "ErrorHandler Destructor." << std::endl;
}

// Log to the console
void ErrorHandler::logError(const std::string &message) {
    std::cerr << "[ErrorHandler]: " << message << std::endl;
}

// Get the error page for a specific status code
std::string ErrorHandler::getErrorPage(int code) {
    if (_errorPages.find(code) != _errorPages.end()) {
        return _errorPages[code]; // Return custom error page if it exists
    }
    return _defaultErrorPage; // Return default error page for unknown status codes
}

// Add a custom error page for a specific status code
void ErrorHandler::setCustomErrorPage(int code, const std::string &pageContent) {
    _errorPages[code] = pageContent;
}
