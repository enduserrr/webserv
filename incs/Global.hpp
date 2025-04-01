/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Global.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 11:10:03 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:39:15 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef LIBS_HPP
# define LIBS_HPP

/* *
 * @brief   Global.hpp has std lib includes, keyword definitions
 *          and log level definitions used along the whole project.
 *
 *          As std lib headers are self-contained transitive include
 *          through Global.hpp works without issues.
 *          (User-defined headers must be directly included to ensure that
 *          all their attributes are forwardable instead of only the parts
 *          that have already been declared.
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


enum class LogLevel {
    INFO = 0,
    WARNING = 1,
    SYS_ERROR = 2,
    DEF_ERR = 3,
    BAD_REQ_ERR = 400,
    FORBIDDEN_REQ_ERR = 403,
    NOT_FOUND_ERR = 404,
    TIME_OUT_ERR = 408,
    PAYLOAD_ERR = 413,
    MEDIA_TYPE_ERR = 415,
    REQ_COUNT_ERR = 429,
    INTERNAL_SERV_ERR = 500,
};

/* COLOR CODES FOR CONSOLE OUTPUTS */
#define GC      "\033[3;90m"
#define GREY_B  "\033[1;90m"
#define YB      "\033[93m"
#define WB      "\033[0;97m"
#define GB      "\033[0;92m"
#define RB      "\033[0;91m"
#define RED     "\033[91m"
#define GRAY    "\033[0;90m"
#define RES     "\033[0m"

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

#endif
