/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 09:06:03 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/04 10:33:29 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

//parsing
#define FILE_SIZE_MAX 1000000000
#define FILE_SIZE_MIN 0


#include <iostream>
#include <string>
// #include <csignal>
// #include <atomic>
#include "ConfParser.hpp"
#include "ServerLoop.hpp"
#include "ErrorHandler.hpp"
#include "HttpParser.hpp"
#include "StaticHandler.hpp"
#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "Webserver.hpp"

//Testing
#include <fstream>
#include <thread>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>

//parsing
#include <sys/stat.h>   // stat
#include <unistd.h>     // access
#include <vector>       // std::vector
#include <sstream>      // stringstream


// Global flag to indicate shutdown request.
// volatile std::sig_atomic_t g_shutdownRequested = 0;

//DEBUG
#define DEBUG 1

#endif
