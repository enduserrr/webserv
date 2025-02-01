/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 09:06:03 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/21 09:06:06 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

//parsing
#define FILE_SIZE_MAX 1000000000
#define FILE_SIZE_MIN 0


#include <iostream>
#include <string>
#include "ConfParser.hpp"
#include "ServerLoop.hpp"
#include "ErrorHandler.hpp"
#include "HttpParser.hpp"
#include "StaticHandler.hpp"

#include <fstream>



//parsing
#include <sys/stat.h>   // stat
#include <unistd.h>     // access
#include <vector>       // std::vector
#include <sstream>      // stringstream




//DEBUG
#define DEBUG 1

#endif
