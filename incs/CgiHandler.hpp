/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/02 10:37:11 by asalo             #+#    #+#             */
/*   Updated: 2025/05/05 11:40:58 by asalo            ###   ########.fr       */
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

// class CgiHandler {
//     private:
//       struct CgiPreparationResult {
//         int pipe_in[2] = {-1, -1};
//         int pipe_out[2] = {-1, -1};
//         char **envp = nullptr;
//         std::vector<const char*> args;
//         std::string phpExecutable;
//         std::string scriptPath;
//         bool success = false;
//     };
//     struct CgiExecutionResult {
//         std::string cgiOutput;
//         int status = 0;
//         bool timedOut = false;
//         bool executionFailed = false; // Indicates failure during fork/pipe/waitpid, not just bad CGI exit code
//         bool childStatusRetrieved = false; // Indicates if waitpid successfully got status info
//     };
//     std::vector<std::string> buildCgiEnvironment(HttpRequest &req);//CGI env vars

//     char **convertEnvVectorToArray(const std::vector<std::string> &env); //Vector of strs to a char* arr
//     std::string executeCgi(const std::string &scriptPath, HttpRequest &req);

//     CgiPreparationResult prepareCgiExecution(HttpRequest &req, const std::string& scriptFullPath);
//     void runChildCgiProcess(CgiPreparationResult& prepData); // Logic for the child process
//     CgiExecutionResult executeParentCgiProcess(pid_t pid, CgiPreparationResult& prepData, HttpRequest& req);
//     std::string buildCgiHttpResponse(CgiExecutionResult& execResult, char** envpToFree);

//     public:
//         CgiHandler();
//         ~CgiHandler();

// 		std::string processRequest(HttpRequest &req);
//     class CgiException : public std::exception {
//     private:
//         std::string message_;

//     public:
//         CgiException(const std::string& message) : message_(message) {}

//         const char* what() const noexcept override {
//             return message_.c_str();
//         }
//     };
// };

class CgiHandler {
    private:
      std::vector<std::string> buildCgiEnvironment(HttpRequest &req);//CGI env vars

      char **convertEnvVectorToArray(const std::vector<std::string> &env); //Vector of strs to a char* arr
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
