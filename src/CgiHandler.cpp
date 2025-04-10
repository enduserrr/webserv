/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:38:49 by asalo             #+#    #+#             */
/*   Updated: 2025/04/10 15:51:38 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

std::string CgiHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    // if (uri.size() < 4 || uri.substr(uri.size() - 4) != ".php") {
    //     return NOT_FOUND + Logger::getInstance().logLevel("ERROR", "CGI failed to process.", 404);
    // }
    std::string script = req.getRoot() + req.getUri();
    std::cout << REV_RED << "src path: " << script << RES << std::endl;
    std::string cgiOutput = executeCgi(script, req);
    return cgiOutput;
}

std::vector<std::string> CgiHandler::buildCgiEnvironment(HttpRequest &req) {
    std::vector<std::string> env;
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("SCRIPT_FILENAME="); // Set later with absolute path
    env.push_back("QUERY_STRING="); // Simplified, assuming no query string, is it even needed??
    env.push_back("CONTENT_LENGTH=" + std::to_string(req.getBody().size()));
    env.push_back("CONTENT_TYPE=application/x-www-form-urlencoded");
    env.push_back("REDIRECT_STATUS=200");
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
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

std::string CgiHandler::executeCgi(const std::string &scriptPath, HttpRequest &req) {
    std::vector<std::string> envVector = buildCgiEnvironment(req);
    envVector[1] = "SCRIPT_FILENAME=" + scriptPath;
    char **envp = convertEnvVectorToArray(envVector);

    std::string phpExecutable = "/usr/bin/php-cgi";
    // std::cout << "PHP executable: " << phpExecutable << std::endl;
    if (access(phpExecutable.c_str(), X_OK) != 0) {
        for (size_t i = 0; i < envVector.size(); ++i) free(envp[i]);
        delete[] envp;
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Access issue", 500);
    }
    if (access(scriptPath.c_str(), F_OK) != 0) {
        for (size_t i = 0; i < envVector.size(); ++i) free(envp[i]);
        delete[] envp;
        return NOT_FOUND + Logger::getInstance().logLevel("ERROR", "Script path", 404);
    }

    std::vector<const char*> args;
    args.push_back(phpExecutable.c_str());
    args.push_back(scriptPath.c_str());
    args.push_back(NULL);

    int pipe_in[2], pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        for (size_t i = 0; i < envVector.size(); ++i) free(envp[i]);
        delete[] envp;
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Pipe", 500);
    }

    pid_t pid = fork();
    if (pid == 0) {
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[1]); close(pipe_out[0]);
        close(pipe_in[0]); close(pipe_out[1]);
        execve(phpExecutable.c_str(), const_cast<char* const*>(args.data()), envp);
        std::cerr << "execve failed: " << strerror(errno) << std::endl;
        exit(1);
    }

    close(pipe_in[0]);
    close(pipe_out[1]);
    if (req.getMethod() == "POST" && !req.getBody().empty()) {
        // std::cout << "Writing POST body: " << req.getBody() << std::endl;
        ssize_t bytes_written = write(pipe_in[1], req.getBody().c_str(), req.getBody().size());
        if (bytes_written == -1) {
            Logger::getInstance().logLevel("ERROR", "ExecuteCgi: write failed (bytes_written == -1)", 0);
        }
    }
    close(pipe_in[1]);

    std::string cgiOutput;
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        cgiOutput.append(buffer);
        // std::cout << "CGI output chunk: " << buffer << std::endl; // Debug
    }
    if (bytes_read < 0) {
        std::cerr << "Read failed: " << strerror(errno) << std::endl;
    }
    close(pipe_out[0]);
    int status;
    waitpid(pid, &status, 0);

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        for (size_t i = 0; i < envVector.size(); ++i) free(envp[i]);
        delete[] envp;
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Issue", 500);
    }

    for (size_t i = 0; i < envVector.size(); ++i) free(envp[i]);
    delete[] envp;

    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << cgiOutput;
    return responseStream.str();
}
