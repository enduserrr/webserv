/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:38:49 by asalo             #+#    #+#             */
/*   Updated: 2025/02/02 13:08:36 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/CgiHandler.hpp"
// #include "../incs/ErrorHandler.hpp"
// #include "../incs/HttpParser.hpp"
// #include <iostream>
#include <sstream>
// #include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

std::string CgiHandler::processRequest(HttpParser &parser) {
    std::string uri = parser.getUri();
    if (uri.size() < 4 || uri.substr(uri.size() - 4) != ".php") {
        return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
            + ErrorHandler::getInstance().getErrorPage(404);
    }
/*  Determine the path to the CGI script. */
/*  For example, assume CGI scripts are stored in "./cgi-bin/" */
    std::string scriptPath = "./cgi-bin" + uri;
    std::string cgiExecutable = "/usr/bin/php-cgi";
    std::string cgiOutput = executeCgi(cgiExecutable, scriptPath, parser);
    /* Build the final HTTP response. */
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\n" << cgiOutput;
    return responseStream.str();
}

std::vector<std::string> CgiHandler::buildCgiEnvironment(HttpParser &parser) {
    std::vector<std::string> env;

    /* REQUEST_METHOD: GET, POST etc */
    env.push_back("REQUEST_METHOD=" + parser.getMethod());

    /* Determine SCRIPT_FILENAME */
    std::string uri = parser.getUri();
    std::string cgiBase = "./cgi-bin";
    std::string scriptFilename;
    if (uri.find("/cgi-bin/") == 0) {
        /* Remove "/cgi-bin" prefix from the URI */
        scriptFilename = cgiBase + uri.substr(8); // 8 is the len of "/cgi-bin"
    } else {
        scriptFilename = cgiBase + uri;
    }
    env.push_back("SCRIPT_FILENAME=" + scriptFilename);

    /* QUERY_STRING from the _uriQuery map */
    std::ostringstream queryStream;
    std::map<std::string, std::string> queryMap = parser.getQueryString();
    bool first = true;
    for (std::map<std::string, std::string>::const_iterator it = queryMap.begin(); it != queryMap.end(); ++it) {
        if (!first) {
            queryStream << "&";
        }
        queryStream << it->first << "=" << it->second;
        first = false;
    }
    env.push_back("QUERY_STRING=" + queryStream.str());
    env.push_back("CONTENT_LENGTH=" + std::to_string(parser.getBody().size()));
    /* Set REDIRECT_STATUS to bypass PHP CGI's security checks */
    env.push_back(std::string("REDIRECT_STATUS=200"));
    return env;
}


char **CgiHandler::convertEnvVectorToArray(const std::vector<std::string> &env) {
    char **envp = new char*[env.size() + 1];
    for (size_t i = 0; i < env.size(); ++i) {
        envp[i] = strdup(env[i].c_str());
    }
    envp[env.size()] = nullptr;
    return envp;
}

/* Build the CGI environment variables */
std::string CgiHandler::executeCgi(const std::string &cgiExecutable,
                                   const std::string &scriptPath,
                                   HttpParser &parser) {
    std::vector<std::string> envVector = buildCgiEnvironment(parser);
    for (std::vector<std::string>::const_iterator it = envVector.begin(); it != envVector.end(); ++it) {
        size_t pos = it->find('=');
        if (pos != std::string::npos) {
            std::string key = it->substr(0, pos);
            std::string value = it->substr(pos + 1);
            setenv(key.c_str(), value.c_str(), 1);
        }
    }
    /* If method is POST and a body exists, using popen() is tricky. Add tmp and redirect. */
    std::string command = cgiExecutable + " " + scriptPath;
    /* popen() to execute the CGI command and open a pipe to read its output */
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               + ErrorHandler::getInstance().getErrorPage(500);
    }
    std::string cgiOutput;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        cgiOutput.append(buffer);
    }
    pclose(pipe);
    /* check returnCode for errors? */
    return cgiOutput;
}
