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

#ifndef LOGGER_HPP
# define LOGGER_HPP

#include "Global.hpp"

class Logger {
    private:
        int                         _state;
        std::map<int, std::string>  _errorPages;
        std::map<int, std::pair<std::string, std::string>>  _responses;
        std::string                 _defaultErrorPage;

        Logger(); // Singleton => private constructor prevents multiple instances
        std::string getCurrentTimestamp() const;
        std::string loadFileContent(const std::string& filePath);

    public:
        ~Logger();
        static Logger& getInstance();

        void        setCustomErrorPage(int code, const std::string &pageContent);
        std::string logLevel(std::string level, const std::string &message, int code);
        std::string getErrorPage(int code);
        std::string getMessage(int code);
        std::string getHeader(int code);

};

#endif

