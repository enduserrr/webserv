/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 09:06:03 by eleppala          #+#    #+#             */
/*   Updated: 2025/03/16 12:57:59 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "Libs.hpp"

#include "ConfParser.hpp"
#include "Location.hpp"
#include "ServerBlock.hpp"
#include "ErrorHandler.hpp"

#include "ServerLoop.hpp"
#include "HttpParser.hpp"
#include "HttpRequest.hpp"

#include "Router.hpp"
#include "Methods.hpp"
#include "StaticHandler.hpp"
#include "CgiHandler.hpp"
#include "UploadHandler.hpp"
#include "Types.hpp"

#define GC      "\033[3;90m"
#define WB      "\033[1;97m"
#define GB      "\033[1;92m"
#define RB      "\033[1;91m"
#define RED     "\033[91m"
#define GRAY    "\033[0;90m"
#define RES     "\033[0m"

#define DEF_CONF "config/default.conf"

//DEBUG
#define DEBUG 1

#endif
