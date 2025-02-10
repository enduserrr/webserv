/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:21:24 by asalo             #+#    #+#             */
/*   Updated: 2025/02/10 10:34:19 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

#include <map>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>

class ErrorHandler {
    private:
        // Private constructor to prevent multiple instances
        ErrorHandler();
        std::map<int, std::string>  _errorPages;
        std::string                 _defaultErrorPage;
        std::string loadFileContent(const std::string &filePath);

    public:
        ~ErrorHandler();

        // Singleton Instance getter function
        static ErrorHandler &getInstance();

        void                logError(const std::string &message);
        std::string         getErrorPage(int code);
        void                setCustomErrorPage(int code, const std::string &pageContent);
};

#endif
