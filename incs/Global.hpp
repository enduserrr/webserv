/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 11:10:03 by asalo             #+#    #+#             */
/*   Updated: 2025/04/09 10:38:49 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef LIBS_HPP
# define LIBS_HPP

/**
 * @brief   Defines global constants, keywords, color codes, and log levels.
 */

#include <iostream>
#include <string>
#include <fstream>
#include <thread>

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <poll.h>
#include <map>
#include <chrono>       // For timestamps
#include <ctime>        // For time formatting
#include <iomanip>      // For put_time
#include <functional>   // For std::function
#include <dirent.h>     // For opendir, readdir, closedir
#include <csignal>      // For signal handling
#include <algorithm>
#include <cctype>
#include <limits.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/types.h>
#include <thread> // WHAT FOR?

/* COLOR CODES FOR CONSOLE OUTPUTS */
#define RES     "\033[0m"
#define GC      "\033[3;90m"
#define GB      "\033[1;90m"
#define GREY    "\033[0;90m"
#define RED     "\033[0;91m"
#define GREEN   "\033[0;92m"
#define YELLOW  "\033[0;93m"
#define BLUE    "\033[0;96m"
#define WB      "\u001b[41;1m"
#define WW      "\033[5m"
#define REV_WHITE "\u001b[47;1;2m"
#define REV_RED  "\u001b[41;1m"

#define OK          "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
#define MOVED       "HTTP/1.1 301 Moved Permanently\r\nContent-Type: text/html\r\n\r\n"
#define BAD_REQ     "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
#define FORBIDDEN   "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html\r\n\r\n"
#define NOT_FOUND   "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
#define NOT_ALLOWED "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: text/html\r\n\r\n"
#define REQ_TIMEOUT "HTTP/1.1 408 Request Time Out\r\nContent-Type: text/html\r\n\r\n"
#define TOO_LARGE   "HTTP/1.1 413 Payload Too Large\r\nContent-Type: text/html\r\n\r\n"
#define UNSUPPORTED "HTTP/1.1 415 Unsupported Media Type\r\nContent-Type: text/html\r\n\r\n"
#define REQ_LIMIT   "HTTP/1.1 429 Too Many Requests\r\nContent-Type: text/html\r\n\r\n"
#define INTERNAL    "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n"

/* SERVER LOOP */
#define MAX_CLIENTS 100

/* DEFINITIONS FOR CONFIGURATION PARSING */
#define DEFAULT_BODY_SIZE 1048576  //1MB
#define FILE_SIZE_MAX 1000000000
#define FILE_SIZE_MIN 0
#define DEF_CONF "config/default.conf"
#define FILE_V "File validation: "
#define CONF "Configuration file: "

/* KEYWORDS FOR CONFIGURATION FILE PARSING */
#define SERVER          "server {"
#define SERVER_NAME     "server_name"
#define PORT            "listen"
#define BODY_SIZE       "client_max_body_size"
#define ERR_PAGE        "error_page"
#define METHODS         "allow_methods"
#define LOCATION        "location"
#define ROOT            "root"
#define AUTOI           "autoindex"
#define INDEX           "index"
#define REDIR           "return"
#define USTORE          "upload_store"

/* HTTP PARSER LIMITS */
#define MAX_REQ_SIZE    16384 //1KB

#endif
