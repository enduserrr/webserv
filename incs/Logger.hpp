/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:21:24 by asalo             #+#    #+#             */
/*   Updated: 2025/03/31 10:42:39 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

// #ifndef ERRORHANDLER_HPP
// # define ERRORHANDLER_HPP

#ifndef LOGGER_HPP
# define LOGGER_HPP

#include "Libs.hpp"
// #include "Webserver.hpp"

// #define INFO
// #define WARNING
// #define ERROR

#define GC      "\033[3;90m"
#define GREY_B  "\033[1;90m"
#define YB      "\033[1;93m"
#define WB      "\033[1;97m"
#define GB      "\033[1;92m"
#define RB      "\033[1;91m"
#define RED     "\033[91m"
#define GRAY    "\033[0;90m"
#define RES     "\033[0m"

// enum class LogLevel {
//     INFO,
//     WARNING,
//     ERROR
// };

class Logger {
    private:
        Logger(); // Singleton => private constructor prevents multiple instances

        int                         _state;
        std::map<int, std::string>  _errorPages;
        std::string                 _defaultErrorPage;
        std::string loadFileContent(const std::string &filePath);

    public:
        ~Logger();

        // Singleton Instance getter function
        static Logger &getInstance();
        std::string getCurrentTimestamp() const;

        std::string         logLevel(std::string level, const std::string &message, int code);
        std::string         getErrorPage(int code);
        void                setCustomErrorPage(int code, const std::string &pageContent);
};

#endif

/* class ErrorHandler {
private:
    ErrorHandler(); // Singleton

    std::map<int, std::string>  _errorPages;
    std::string                 _defaultErrorPage;

    // Deleted copy constructor and assignment operator for Singleton => useless I think?
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;

    std::string loadFileContent(const std::string &filePath);
    std::string logLevelToString(LogLevel level) const; // Convert enum to string
    std::string getCurrentTimestamp() const; // Get formatted timestamp

public:
    ~ErrorHandler();
    static ErrorHandler &getInstance();
    void log(LogLevel level, const std::string &message);

    std::string handleHttpError(int httpStatusCode, const std::string &errorMessage);

    std::string getErrorPageContent(int httpStatusCode);

    void setCustomErrorPage(int httpStatusCode, const std::string &pagePath);

    void setCustomErrorPageContent(int httpStatusCode, const std::string &pageContent);
};

#endif */

