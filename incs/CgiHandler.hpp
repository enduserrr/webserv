/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:37:11 by asalo             #+#    #+#             */
/*   Updated: 2025/03/31 10:43:13 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "HttpParser.hpp"
#include "Logger.hpp"
#include "UploadHandler.hpp"
#include "HttpRequest.hpp"
// #include <vector>
// #include <string>
#include "Libs.hpp"

class CgiHandler {
    private:
		std::vector<std::string> buildCgiEnvironment(HttpRequest &req);// Build CGI env vars
		char **convertEnvVectorToArray(const std::vector<std::string> &env); //Convert vector of strs to a char* arr
		std::string executeCgi(const std::string &cgiExecutable,
							const std::string &scriptPath,
							HttpRequest &req);//Execute the CGI script and return its output.

    public:
        CgiHandler();
        ~CgiHandler();

		std::string processRequest(HttpRequest &req);
};

#endif
