/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:21:14 by asalo             #+#    #+#             */
/*   Updated: 2025/01/26 16:21:17 by asalo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../incs/ErrorHandler.hpp"

ErrorHandler::ErrorHandler() {
    _defaultErrorPages[400] = "<html><body><h1>400 Bad Request</h1><p>Your request cannot be processed.</p></body></html>";
    _defaultErrorPages[404] = "<html><body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>";
    _defaultErrorPages[500] = "<html><body><h1>500 Internal Server Error</h1><p>An unexpected error occurred.</p></body></html>";
    _defaultErrorPages[403] = "<html><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>";
    _defaultErrorPages[408] = "<html><body><h1>408 Request Timeout</h1><p>The server timed out waiting for the request.</p></body></html>";
}

ErrorHandler::~ErrorHandler() {}

void ErrorHandler::logError(const std::string &message) {
    std::cerr << "[ErrorHandler] " << message << std::endl;
}

void ErrorHandler::setErrorPage(int status, const std::string &path) {
    _errorPages[status] = path;
}

std::string ErrorHandler::getErrorPage(int status) {
    if (_errorPages.find(status) != _errorPages.end()) {
        std::ifstream file(_errorPages[status]);
        if (file.is_open()) {
            return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        } else {
            logError("Failed to open custom error page: " + _errorPages[status]);
        }
    }
    if (_defaultErrorPages.find(status) != _defaultErrorPages.end()) {
        return _defaultErrorPages[status];
    }
    // return the error message if the status code exists or a generic error
    return "<html><body><h1>Error</h1><p>An unknown error occurred. :D</p></body></html>";
}
