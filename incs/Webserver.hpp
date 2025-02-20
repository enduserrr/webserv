/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 09:06:03 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/20 10:51:03 by asalo            ###   ########.fr       */
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

//DEBUG
#define DEBUG 1

#endif
