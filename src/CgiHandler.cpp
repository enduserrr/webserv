/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:38:49 by asalo             #+#    #+#             */
/*   Updated: 2025/04/02 19:19:15 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}

CgiHandler::~CgiHandler() {}

std::string CgiHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    if (uri.size() < 4 || uri.substr(uri.size() - 4) != ".php") {
        return "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
               + Logger::getInstance().logLevel("ERROR", "Unable to process cgi request.", 404);
    }

    std::string scriptPath = req.getRoot() + uri;
    std::string cgiOutput;

    try {
        cgiOutput = executeCgi(scriptPath, req);
        return cgiOutput;
    } catch (const CgiException &e) {

         return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                +  Logger::getInstance().logLevel("ERROR", e.what(), 500);
    } catch (const std::exception &e) {

         return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                + Logger::getInstance().logLevel("ERROR", e.what(), 500);
    } catch (...) {

        return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
               + Logger::getInstance().logLevel("ERROR", "Unknown error during CGI execution", 500);
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

    // PHP executable path
    std::string phpExecutable = "/usr/bin/php";

    // Construct the command
    std::vector<const char*> args;
    args.push_back(phpExecutable.c_str());
    args.push_back(scriptPath.c_str());
    args.push_back(NULL);
    // pieps for communication
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

        // If execve fails - Another solution?
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
                kill(pid, SIGKILL);
                waitpid(pid, nullptr, 0);
                return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                       "<h1>500 Internal Server Error</h1><p>Error writing to pipe.</p>";
            }
        }
        close(pipe_in[1]);

        // Read the CGI output from the pipe
        std::string cgiOutput;
        char buffer[1024];
        ssize_t bytes_read;

        // Set timeout
        const int timeout_seconds = 5;
        auto start = std::chrono::steady_clock::now();

        while (true) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(pipe_out[0], &readfds);

            struct timeval timeout;
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000;  // Check every 100 milliseconds

            int select_result = select(pipe_out[0] + 1, &readfds, NULL, NULL, &timeout);
            // REPLACE
            if (select_result == -1) {
                if (errno == EINTR) continue;  // Interrupted by signal, try again

                Logger::getInstance().logLevel("ERROR", "Error with select", 500);
                close(pipe_out[0]);
                kill(pid, SIGKILL);
                waitpid(pid, nullptr, 0);

                return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                       "<h1>500 Internal Server Error</h1><p>Error reading from pipe (select).</p>";
            } else if (select_result > 0) {
                bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1);
                if (bytes_read > 0) {
                    buffer[bytes_read] = 0; // Null-terminate
                    cgiOutput.append(buffer);
                } else if (bytes_read == 0) {
                    break ; // EOF
                } else {
                     Logger::getInstance().logLevel("ERROR", "Error reading from pipe.", 500);
                    close(pipe_out[0]);
                    kill(pid, SIGKILL);
                    waitpid(pid, nullptr, 0);

                    return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                           "<h1>500 Internal Server Error</h1><p>Error reading from pipe.</p>";

                }

            }
            // Consider not using auto
            auto now = std::chrono::steady_clock::now();
            auto elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

            if (elapsed_seconds >= timeout_seconds) {
                Logger::getInstance().logLevel("ERROR", "CGI script execution timed out.", 508);
                close(pipe_out[0]);
                kill(pid, SIGKILL);  // Kill the child process
                waitpid(pid, nullptr, 0); // Wait for the child to terminate

                return "HTTP/1.1 508 Gateway Timeout\r\nContent-Type: text/html\r\n\r\n"
                       "<h1>508 Gateway Timeout</h1><p>CGI script execution timed out.</p>";
            }

             if (waitpid(pid, nullptr, WNOHANG) != 0)
             {
                break ;
             }

        }

        close(pipe_out[0]);

        int status;
        waitpid(pid, &status, 0);

        // Free allocated memory.
        for (size_t i = 0; i < envVector.size(); ++i) {
            free(envp[i]);
        }
        delete[] envp;

        //check if exited with correct status, but timeout takes presedence.
       if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start).count() < timeout_seconds
       && (!WIFEXITED(status) || WEXITSTATUS(status) != 0)) {
            return "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"
                   + Logger::getInstance().logLevel("ERROR", "Internal server error.  Error reading CGI response.", 500);
       }

        // Parse headers and body
        size_t headerEndPos = cgiOutput.find("\r\n\r\n"); // Find end of headers

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

        // Build the HTTP response
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
