/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:41:08 by asalo             #+#    #+#             */
/*   Updated: 2025/02/24 11:13:51 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/StaticHandler.hpp"
#include "../incs/ErrorHandler.hpp"
#include "../incs/UploadHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

std::string StaticHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string method = req.getMethod();

    /* If this is a POST request with a body, treat it as a file upload */
    if (method == "POST" && !req.getBody().empty())
        return Methods::mPost(req);
    if (method == "DELETE")
        return Methods::mDelete(req);
    return Methods::mGet(req);
}
