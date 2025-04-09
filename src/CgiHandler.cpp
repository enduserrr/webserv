/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:38:49 by asalo             #+#    #+#             */
/*   Updated: 2025/04/09 10:54:20 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

// std::string CgiHandler::processRequest(HttpRequest &req) {
//     std::string uri = req.getUri();
//     if (uri.size() < 4 || uri.substr(uri.size() - 4) != ".php") {
//         return INTERNAL + Logger::getInstance().logLevel("ERROR", "Unable to process cgi request.", 404);
//     }

//     std::string scriptPath = req.getRoot() + uri;
//     std::string cgiOutput;

//     try {
//         cgiOutput = executeCgi(scriptPath, req);
//         return cgiOutput;
//     } catch (const CgiException &e) {

//          return INTERNAL + Logger::getInstance().logLevel("ERROR", e.what(), 500);
//     } catch (const std::exception &e) {

//          return INTERNAL + Logger::getInstance().logLevel("ERROR", e.what(), 500);
//     } catch (...) {

//         return INTERNAL + Logger::getInstance().logLevel("ERROR", "Unknown error during CGI execution", 500);
//     }
// }

std::string CgiHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::cout << "CGI URI: " << uri << std::endl; // Debug log
    if (uri.size() < 4 || uri.substr(uri.size() - 4) != ".php") {
        return NOT_FOUND + Logger::getInstance().logLevel("ERROR", "CGI failed to process.", 404); // Use 404 consistently
    }

    std::string scriptPath = req.getRoot() + uri;
    std::cout << "Script path: " << scriptPath << std::endl; // Debug log

    std::string cgiOutput = executeCgi(scriptPath, req); // Simplified, let executeCgi handle exceptions
    return cgiOutput;
}

// std::vector<std::string> CgiHandler::buildCgiEnvironment(HttpRequest &req) {
//     std::vector<std::string> env;

//     env.push_back("REQUEST_METHOD=" + req.getMethod());
//     env.push_back("SCRIPT_FILENAME=." + req.getUri());

//     // QUERY_STRING from the map
//     std::ostringstream queryStream;
//     const std::map<std::string, std::string>& queryMap = req.getUriQuery();
//     bool first = true;
//     for (std::map<std::string, std::string>::const_iterator it = queryMap.begin(); it != queryMap.end(); ++it) {
//         if (!first) {
//             queryStream << "&";
//         }
//         queryStream << it->first << "=" << it->second;
//         first = false;
//     }
//     env.push_back("QUERY_STRING=" + queryStream.str());

//     env.push_back("CONTENT_LENGTH=" + std::to_string(req.getBody().size()));
//     env.push_back("REDIRECT_STATUS=200");

//     return env;
// }

std::vector<std::string> CgiHandler::buildCgiEnvironment(HttpRequest &req) {
    std::vector<std::string> env;
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("SCRIPT_FILENAME="); // Set later with absolute path
    env.push_back("QUERY_STRING="); // Simplified, assuming no query string
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
    envVector[1] = "SCRIPT_FILENAME=" + scriptPath; // Use absolute path
    char **envp = convertEnvVectorToArray(envVector);

    std::string phpExecutable = "/usr/bin/php-cgi"; // Try php-cgi instead of php
    std::cout << "PHP executable: " << phpExecutable << std::endl;
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
        std::cerr << "execve failed: " << strerror(errno) << std::endl; // Logger::
        exit(1);
    }

    close(pipe_in[0]);
    close(pipe_out[1]);
    if (req.getMethod() == "POST" && !req.getBody().empty()) {
        std::cout << "Writing POST body: " << req.getBody() << std::endl;
        ssize_t bytes_written = write(pipe_in[1], req.getBody().c_str(), req.getBody().size());
        if (bytes_written == -1) {
            std::cerr << "Write failed: " << strerror(errno) << std::endl; // Logger::
        }
    }
    close(pipe_in[1]);

    std::string cgiOutput;
    char buffer[1024];
    ssize_t bytes_read;
    while ((bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        cgiOutput.append(buffer);
        std::cout << "CGI output chunk: " << buffer << std::endl; // Debug
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
        return INTERNAL + Logger::getInstance().logLevel("ERROR", "Issue", 500); // Likely source of your error
    }

    for (size_t i = 0; i < envVector.size(); ++i) free(envp[i]);
    delete[] envp;

    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" << cgiOutput;
    return responseStream.str();
}

// std::string CgiHandler::executeCgi(const std::string &scriptPath, HttpRequest &req) {
//     std::vector<std::string> envVector = buildCgiEnvironment(req);
//     char **envp = convertEnvVectorToArray(envVector);

//     // PHP executable path
//     std::string phpExecutable = "/usr/bin/php";

//     // Construct the command
//     std::vector<const char*> args;
//     args.push_back(phpExecutable.c_str());
//     args.push_back(scriptPath.c_str());
//     args.push_back(NULL);
//     // pieps for communication
//     int pipe_in[2];
//     int pipe_out[2];
//     if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
//         return INTERNAL + Logger::getInstance().logLevel("ERROR", "Failed to create pipes.", 500);
//     }

//     pid_t pid = fork();
//     if (pid == -1) {
//         close(pipe_in[0]); close(pipe_in[1]);
//         close(pipe_out[0]); close(pipe_out[1]);
//         return INTERNAL + Logger::getInstance().logLevel("ERROR", "Failed to fork process.", 500);
//     }
//     if (pid == 0) {  // Child process
//         // Redirect stdin and stdout
//         dup2(pipe_in[0], STDIN_FILENO);
//         dup2(pipe_out[1], STDOUT_FILENO);

//         close(pipe_in[1]);
//         close(pipe_out[0]);

//         close(pipe_in[0]);
//         close(pipe_out[1]);

//         // Execute script
//         execve(phpExecutable.c_str(), const_cast<char* const*>(args.data()), envp);

//         // If execve fails - Another solution?
//         perror("execve");
//         for (size_t i = 0; i < envVector.size(); ++i) {
//             free(envp[i]);
//         }
//         delete[] envp;
//         exit(1); // Exit the child process on error
//     } else {  // Parent process
//         close(pipe_in[0]);
//         close(pipe_out[1]);
//         // Send the body to the CGI script's stdin
//         if (req.getMethod() == "POST" && !req.getBody().empty()) {
//             ssize_t bytes_written = write(pipe_in[1], req.getBody().c_str(), req.getBody().size());
//             if (bytes_written == -1) {
//                 close(pipe_in[1]); close(pipe_out[0]);
//                 kill(pid, SIGKILL);
//                 waitpid(pid, nullptr, 0);
//                 return INTERNAL + Logger::getInstance().logLevel("ERROR", "Error reading CGI response. Error writing to pipe.", 500);
//             }
//         }
//         close(pipe_in[1]);

//         // Read the CGI output from the pipe
//         std::string cgiOutput;
//         char buffer[1024];
//         ssize_t bytes_read;

//         // Set timeout
//         const int timeout_seconds = 5;
//         std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

//         while (true) {
//             fd_set readfds;
//             FD_ZERO(&readfds);
//             FD_SET(pipe_out[0], &readfds);

//             struct timeval timeout;
//             timeout.tv_sec = 0;
//             timeout.tv_usec = 100000;  // Check every 100 milliseconds

//             int select_result = select(pipe_out[0] + 1, &readfds, NULL, NULL, &timeout);
//             // REPLACE
//             if (select_result == -1) {
//                 if (errno == EINTR)
//                     continue;  // Interrupted by signal, try again
//                 close(pipe_out[0]);
//                 kill(pid, SIGKILL);
//                 waitpid(pid, nullptr, 0);
//                 return INTERNAL + Logger::getInstance().logLevel("ERROR", "Error reading CGI response. Error reading from pipe (select)", 500);
//             } else if (select_result > 0) {
//                 bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1);
//                 if (bytes_read > 0) {
//                     buffer[bytes_read] = 0; // Null-terminate
//                     cgiOutput.append(buffer);
//                 } else if (bytes_read == 0) {
//                     break ; // EOF
//                 } else {
//                     close(pipe_out[0]);
//                     kill(pid, SIGKILL);
//                     waitpid(pid, nullptr, 0);

//                     return INTERNAL + Logger::getInstance().logLevel("ERROR", "Error reading CGI response. Error reading from pipe.", 500);

//                 }

//             }
//             std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
//             long long elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
//             if (elapsed_seconds >= timeout_seconds) {
//                 close(pipe_out[0]);
//                 kill(pid, SIGKILL);  // Kill the child process
//                 waitpid(pid, nullptr, 0); // Wait for the child to terminate
//                 return INTERNAL + Logger::getInstance().logLevel("ERROR", "Internal server error.", 500);
//             }
//              if (waitpid(pid, nullptr, WNOHANG) != 0)
//              {
//                 break ;
//              }
//         }

//         close(pipe_out[0]);
//         int status;
//         waitpid(pid, &status, 0);
//         // Free allocated memory.
//         for (size_t i = 0; i < envVector.size(); ++i) {
//             free(envp[i]);
//         }
//         delete[] envp;
//         //check if exited with correct status, but timeout takes presedence.
//        if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < timeout_seconds
//        && (!WIFEXITED(status) || WEXITSTATUS(status) != 0)) {
//             return INTERNAL + Logger::getInstance().logLevel("ERROR", "Internal server error. Error reading CGI response.", 500);
//        }

//         // Parse headers and body
//         size_t headerEndPos = cgiOutput.find("\r\n\r\n"); // Find end of headers
//         std::string headers;
//         std::string body;
//         if (headerEndPos != std::string::npos) {
//             headers = cgiOutput.substr(0, headerEndPos);
//             body = cgiOutput.substr(headerEndPos + 4); // Skip the \r\n\r\n
//         } else {
//             // Treat the entire output as the body if no headers are found.
//             body = cgiOutput;
//             headers = "";
//         }

//         // Build the HTTP response
//         std::ostringstream responseStream;
//         responseStream << "HTTP/1.1 200 OK\r\n";
//         //Add Content-Type header
//         if (headers.find("Content-Type:") == std::string::npos) {
//             responseStream << "Content-Type: text/html\r\n"; // Default if not specified
//         }
//         responseStream << headers << "\r\n\r\n";
//         responseStream << body;

//         return responseStream.str();
//     }
// }
