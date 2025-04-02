/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:38:49 by asalo             #+#    #+#             */
/*   Updated: 2025/04/02 18:32:57 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

#include <chrono>
#include <csignal>
#include <setjmp.h>

static sigjmp_buf jmpbuf;
static volatile bool timeout_flag = false;

static void timeout_handler(int sig) {
    (void)sig;
    timeout_flag = true;
    siglongjmp(jmpbuf, 1);
}

std::string CgiHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    if (uri.size() < 4 || uri.substr(uri.size() - 4) != ".php") {
        return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
               + Logger::getInstance().logLevel("ERROR", "Unable to process cgi request.", 404);
    }

    std::string scriptPath = req.getRoot() + uri;
    std::string cgiOutput;

    struct sigaction sa;
    sa.sa_handler = timeout_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        Logger::getInstance().logLevel("ERROR", "Failed to set signal handler.", 500);
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               "<h1>500 Internal Server Error</h1><p>Failed to set signal handler.</p>";
    }

    alarm(5);
    timeout_flag = false;

    if (sigsetjmp(jmpbuf, 1) == 0) {
        cgiOutput = executeCgi(scriptPath, req);
        alarm(0);

        std::ostringstream responseStream;
        responseStream << "HTTP/1.1 200 OK\r\n" << cgiOutput;
        return responseStream.str();
    } else {
        alarm(0);
        return "HTTP/1.1 508 Gateway Timeout\r\nContent-Type: text/html\r\n\r\n" +
               Logger::getInstance().logLevel("ERROR", "CGI script execution timed out.", 408);
    }
}

std::vector<std::string> CgiHandler::buildCgiEnvironment(HttpRequest &req) {
    std::vector<std::string> env;

    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("SCRIPT_FILENAME=." + req.getUri());

    // QUERY_STRING from the map
    std::ostringstream queryStream;
    const std::map<std::string, std::string>& queryMap = req.getUriQuery();
    bool first = true;
    for (std::map<std::string, std::string>::const_iterator it = queryMap.begin(); it != queryMap.end(); ++it) {
        if (!first) {
            queryStream << "&";
        }
        queryStream << it->first << "=" << it->second;
        first = false;
    }
    env.push_back("QUERY_STRING=" + queryStream.str());

    env.push_back("CONTENT_LENGTH=" + std::to_string(req.getBody().size()));
    env.push_back("REDIRECT_STATUS=200");

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
    char **envp = convertEnvVectorToArray(envVector);

    // Meaby make this configurable for more script types
    std::string phpExecutable = "/usr/bin/php"; // 'which php'

    // Construct the command to execute script
    std::vector<const char*> args;
    args.push_back(phpExecutable.c_str());
    args.push_back(scriptPath.c_str());
    args.push_back(NULL); // execve requires a NULL-terminated argument list

    int pipe_in[2];
    int pipe_out[2];
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        Logger::getInstance().logLevel("ERROR", "Failed to create pipes.", 500);
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               "<h1>500 Internal Server Error</h1><p>Failed to create pipes.</p>";
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipe_in[0]); close(pipe_in[1]);
        close(pipe_out[0]); close(pipe_out[1]);
        Logger::getInstance().logLevel("ERROR", "Failed to fork process.", 500);
        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               "<h1>500 Internal Server Error</h1><p>Failed to fork process.</p>";
    }

    if (pid == 0) {  // Child process
        // Redirect stdin and stdout
        dup2(pipe_in[0], STDIN_FILENO);
        dup2(pipe_out[1], STDOUT_FILENO);
        close(pipe_in[1]);
        close(pipe_out[0]);

        close(pipe_in[0]);
        close(pipe_out[1]);

        // Execute script
        execve(phpExecutable.c_str(), const_cast<char* const*>(args.data()), envp);

        // If execve fails
        perror("execve"); // Print the error message

        for (size_t i = 0; i < envVector.size(); ++i) {
            free(envp[i]);
        }
        delete[] envp;
        exit(1); // Exit the child process on error
    } else {  // Parent process
        close(pipe_in[0]);
        close(pipe_out[1]);

        // Send the body to the CGI script's stdin
        if (req.getMethod() == "POST" && !req.getBody().empty()) {
            ssize_t bytes_written = write(pipe_in[1], req.getBody().c_str(), req.getBody().size());
            if (bytes_written == -1) {
                Logger::getInstance().logLevel("ERROR", "Error writing to pipe.", 500);
                close(pipe_in[1]); close(pipe_out[0]);
                return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                       "<h1>500 Internal Server Error</h1><p>Error writing to pipe.</p>";
            }
        }
        close(pipe_in[1]);

        // Read the CGI output from the pipe
        std::string cgiOutput;
        char buffer[1024];
        ssize_t bytes_read;
        while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = 0;
            cgiOutput.append(buffer);
        }

        close(pipe_out[0]);

        int status;
        waitpid(pid, &status, 0);

        for (size_t i = 0; i < envVector.size(); ++i) {
            free(envp[i]);
        }
        delete[] envp;

        if (bytes_read == -1 || !WIFEXITED(status) || WEXITSTATUS(status) != 0 )
        {
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                   + Logger::getInstance().logLevel("ERROR", "Internal server error.  Error reading CGI response.", 500);
        }

        // Parse headers and body (This is a SIMPLIFIED header parsing.
        size_t headerEndPos = cgiOutput.find("\r\n\r\n");

        std::string headers;
        std::string body;
        if (headerEndPos != std::string::npos) {
            headers = cgiOutput.substr(0, headerEndPos);
            body = cgiOutput.substr(headerEndPos + 4); // Skip the \r\n\r\n
        } else {
            // Treat the entire output as the body if no headers are found.
            body = cgiOutput;
            headers = "";
        }

        std::ostringstream responseStream;
        responseStream << "HTTP/1.1 200 OK\r\n";
        //Add Content-Type header
        if (headers.find("Content-Type:") == std::string::npos) {
            responseStream << "Content-Type: text/html\r\n"; // Default if not specified
        }
        responseStream << headers << "\r\n\r\n";
        responseStream << body;

        return responseStream.str();
    }
}
