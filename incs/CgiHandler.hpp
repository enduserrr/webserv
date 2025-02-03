/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:37:11 by asalo             #+#    #+#             */
/*   Updated: 2025/02/03 11:01:09 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include "HttpParser.hpp"
#include "ErrorHandler.hpp"
#include "UploadHandler.hpp"
#include <vector>
#include <string>

class CgiHandler {
    private:
		std::vector<std::string> buildCgiEnvironment(HttpParser &parser);// Build CGI env vars
		char **convertEnvVectorToArray(const std::vector<std::string> &env); //Convert vector of strs to a char* arr
		std::string executeCgi(const std::string &cgiExecutable,
							const std::string &scriptPath,
							HttpParser &parser);//Execute the CGI script and return its output.

    public:
        CgiHandler();
        ~CgiHandler();

		std::string processRequest(HttpParser &parser);
};

#endif
