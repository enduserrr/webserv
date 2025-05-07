/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:38:49 by asalo             #+#    #+#             */
/*   Updated: 2025/05/06 11:57:56 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

/**
 * @brief   Entry point that gets the script path
 *          and calls executeCgi to handle the request.
 */
std::string CgiHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string script = req.getRoot() + req.getUri();
    std::string cgiOutput = executeCgi(script, req);
    return cgiOutput;
}

/**
 * @brief   Creates a vector of standard CGI environment
 *          variable strings based on the HttpRequest.
 *          ENV's used to pass request info to the script processing
 */
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

/**
 * @brief   Converts the C++ vector of environment strings to
 *          the char array format as required by execve.
 */
char **CgiHandler::convertEnvVectorToArray(const std::vector<std::string> &env) {
    char **envp = new char*[env.size() + 1];
    for (size_t i = 0; i < env.size(); ++i) {
        envp[i] = strdup(env[i].c_str());
    }
    envp[env.size()] = nullptr;
    return envp;
}

/**
 * @brief   Setup cgi env, create pipes for parent/child comms, forks a child process,
 *          child process pipe handling, parent process (closes unused pipes), writes the request to childs pipe,
 *          in parent loo: checks for timeout, checks if child has exited, check it's output with poll, takes the output
 *          from child out pipe, makes a response
 */
std::string CgiHandler::executeCgi(const std::string &scriptPath, HttpRequest &req) {
    std::vector<std::string> envVector = buildCgiEnvironment(req);
    size_t queryPos = req.getUri().find('?');
    envVector[2] = (queryPos != std::string::npos) ? ("QUERY_STRING=" + req.getUri().substr(queryPos + 1)) : "QUERY_STRING=";
    envVector[1] = "SCRIPT_FILENAME=" + scriptPath;
    char **envp = convertEnvVectorToArray(envVector);
    std::string phpExecutable = "/usr/bin/php-cgi"; // Consider making configurable
    if (access(phpExecutable.c_str(), X_OK) != 0) {
        for (size_t i = 0; envp[i] != nullptr; ++i) free(envp[i]);
        delete[] envp;
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "CGI executable access issue", 500);
    }
    if (access(scriptPath.c_str(), R_OK) != 0) {
        for (size_t i = 0; envp[i] != nullptr; ++i) free(envp[i]);
        delete[] envp;
        return NOT_FOUND + Logger::getInstance().logLevel("ERROR", "Script path not found or unreadable", 404);
    }

    std::vector<const char*> args;
    args.push_back(phpExecutable.c_str());
    args.push_back(scriptPath.c_str());
    args.push_back(NULL);

    int pipe_in[2], pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        for (size_t i = 0; envp[i] != nullptr; ++i) free(envp[i]);
        delete[] envp;
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Pipe creation failed", 500);
    }

    pid_t pid = fork();
    if (pid == -1) { // Fork failed check
        close(pipe_in[0]); close(pipe_in[1]); close(pipe_out[0]); close(pipe_out[1]);
        for (size_t i = 0; envp[i] != nullptr; ++i) free(envp[i]); delete[] envp;
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Fork failed", 500);
    }

    if (pid == 0) { // Child
        dup2(pipe_in[0], STDIN_FILENO); dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[1]); close(pipe_out[0]); close(pipe_in[0]); close(pipe_out[1]);
        execve(phpExecutable.c_str(), const_cast<char* const*>(args.data()), envp);
        Logger::getInstance().logLevel("ERROR", "CGI execve failed for " + scriptPath, 500);
        exit(EXIT_FAILURE);
    }

    // Parent
    close(pipe_in[0]); close(pipe_out[1]);

    if (req.getMethod() == "POST" && !req.getBody().empty()) {
        ssize_t total_written = 0; ssize_t bytes_written;
        const char* body_ptr = req.getBody().c_str(); size_t body_size = req.getBody().size();
        while (total_written < (ssize_t)body_size) {
             bytes_written = write(pipe_in[1], body_ptr + total_written, body_size - total_written);
             if (bytes_written <= 0) {
                 if (bytes_written < 0) Logger::getInstance().logLevel("ERROR", "CGI write to pipe_in failed", 0);
                 break ;
             }
             total_written += bytes_written;
        }
    }
    close(pipe_in[1]);

    std::string cgiOutput;
    char buffer[4096];
    ssize_t bytes_read;
    int status = 0;
    bool timedOut = false;
    bool childExited = false;

    time_t startTime = time(nullptr);
    const time_t cgiTimeoutSeconds = 5;

     // ↓↓↓ READ & CHECK CHILD STATUS/TIMEOUT ↓↓↓
    while (true) {
        // ↓↓↓ 1. OVERALL TIMEOUT ↓↓↓
        if (!timedOut && (time(nullptr) - startTime >= cgiTimeoutSeconds)) {
            Logger::getInstance().logLevel("WARNING", "CGI script timed out", 504);
            kill(pid, SIGKILL); // Kill the child process forcefully
            timedOut = true;
        }
        // ↓↓↓ 2. HAS CHILD EXITED (NON BLOCKING) ↓↓↓
        int wait_ret = waitpid(pid, &status, WNOHANG);
        if (wait_ret == pid) {
            childExited = true;
            break ;
        } else if (wait_ret < 0) {
            Logger::getInstance().logLevel("ERROR", "Error during CGI loop", 500);
            break ;
        }
        // ↓↓↓ 3. POLL READ PIPE WITH 100MS TIMEOUT ↓↓↓
        struct pollfd pfd; pfd.fd = pipe_out[0]; pfd.events = POLLIN;
        int poll_ret = poll(&pfd, 1, 100);

        if (poll_ret > 0) {
            if (pfd.revents & POLLIN) {
                bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0'; cgiOutput.append(buffer);
                } else {
                    if (bytes_read < 0) Logger::getInstance().logLevel("ERROR", "Read failed for CGI pipe_out", 500);
                    break ; // EOF or read error, exit loop
                }
            } else if (pfd.revents & (POLLHUP | POLLERR)) { break; } // Pipe closed/error
        } else if (poll_ret < 0) {
             Logger::getInstance().logLevel("ERROR", "Poll failed during CGI read", 500);
             break ;
        }
        if (timedOut) {
            break ;
        }
    }

    close(pipe_out[0]);

    if (!childExited) {
        int final_wait_ret = waitpid(pid, &status, 0);
        if (final_wait_ret == pid) {
            childExited = true;
        } else if (final_wait_ret < 0) {
            Logger::getInstance().logLevel("ERROR", "Final blocking waitpid failed", 500);
        }
    }

    // ↓↓↓ FREE ENV VARIABLES ↓↓↓
    for (size_t i = 0; envp[i] != nullptr; ++i) free(envp[i]); delete[] envp;

    // ↓↓↓ CHECK FOR TIMEOUT ↓↓↓
    if (timedOut) {
        return GATEWAY_TIMEOUT + Logger::getInstance().logLevel("ERROR", "Gateway Timeout", 504);
    }

    // ↓↓↓ CHECK IF STATUS RETRIEVED (childExited == true) & DIDN'T TIMEOUT ↓↓↓
    if (!childExited || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        std::ostringstream logStream;
        logStream << "CGI script execution failed or returned non-zero.";
        if (childExited) { // Log details only if we have valid status
             if (WIFSIGNALED(status)) { logStream << " Terminated by signal: " << WTERMSIG(status); }
             else if (WIFEXITED(status)) { logStream << " Exit status: " << WEXITSTATUS(status); }
        } else {
             // Happens only if the final blocking waitpid failed earlier
             logStream << " Status could not be determined (waitpid failed).";
        }
        Logger::getInstance().logLevel("ERROR", logStream.str(), 500);
        return INTERNAL;
    }

    std::string::size_type header_end = cgiOutput.find("\r\n\r\n");
    std::string cgiBody = cgiOutput;
    std::string contentType = "text/html"; std::string statusCode = "200 OK";
    if (header_end != std::string::npos) {
        std::string headers_part = cgiOutput.substr(0, header_end);
        cgiBody = cgiOutput.substr(header_end + 4);
        std::string::size_type ct_pos = headers_part.find("Content-Type: ");
        if (ct_pos != std::string::npos) { std::string::size_type ct_end = headers_part.find("\r\n", ct_pos); contentType = (ct_end != std::string::npos) ? headers_part.substr(ct_pos + 14, ct_end - (ct_pos + 14)) : headers_part.substr(ct_pos + 14); }
        std::string::size_type st_pos = headers_part.find("Status: ");
        if (st_pos != std::string::npos) { std::string::size_type st_end = headers_part.find("\r\n", st_pos); statusCode = (st_end != std::string::npos) ? headers_part.substr(st_pos + 8, st_end - (st_pos + 8)) : headers_part.substr(st_pos + 8); }
    }

    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << statusCode << "\r\n"
                   << "Content-Length: " << cgiBody.size() << "\r\n"
                   << "Content-Type: " << contentType << "\r\n" << "\r\n" << cgiBody;
    return responseStream.str();
}