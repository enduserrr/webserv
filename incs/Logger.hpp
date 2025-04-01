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
        Logger(); // Singleton => private constructor prevents multiple instances

        int                         _state;
        std::map<int, std::string>  _errorPages;
        std::string                 _defaultErrorPage;

        std::string loadFileContent(const std::string &filePath);

    public:
        ~Logger();
        static Logger &getInstance();
        std::string getCurrentTimestamp() const;

        std::string         logLevel(std::string level, const std::string &message, int code);
        std::string         getErrorPage(int code);
        void                setCustomErrorPage(int code, const std::string &pageContent);
};

#endif

