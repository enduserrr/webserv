/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:37:11 by asalo             #+#    #+#             */
/*   Updated: 2025/05/06 09:19:21 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "Global.hpp"
#include "HttpParser.hpp"
#include "Logger.hpp"
#include "UploadHandler.hpp"
#include "HttpRequest.hpp"
#include "Types.hpp"

class CgiHandler {
    private:
      std::vector<std::string> buildCgiEnvironment(HttpRequest &req);

      char **convertEnvVectorToArray(const std::vector<std::string> &env);
      std::string executeCgi(const std::string &scriptPath, HttpRequest &req);

    public:
        CgiHandler();
        ~CgiHandler();

		std::string processRequest(HttpRequest &req);
    class CgiException : public std::exception {
    private:
        std::string message_;

    public:
        CgiException(const std::string& message) : message_(message) {}

        const char* what() const noexcept override {
            return message_.c_str();
        }
    };
};

#endif
