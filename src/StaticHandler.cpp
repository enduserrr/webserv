/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:41:08 by asalo             #+#    #+#             */
/*   Updated: 2025/05/05 18:09:56 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "StaticHandler.hpp"

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

/**
 * @brief   Calls for correct function to handle the static request being processed.
 */
std::string StaticHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string method = req.getMethod();
    
    if (method == "POST") {
        if (req.getBody().empty()) {
            Logger::getInstance().logLevel("ERROR", "POST request has an empty body!", 500);
        } else {
            return Methods::mPost(req);
        }
    }
    if (method == "DELETE") {
        return Methods::mDelete(req);
    }
    return Methods::mGet(req);
}

