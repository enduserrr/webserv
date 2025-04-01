/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:41:08 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:34:28 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "StaticHandler.hpp"

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

std::string StaticHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string method = req.getMethod();

    if (method == "POST") {
        if (req.getBody().empty()) {
            Logger::getInstance().logLevel("ERROR", "POST request has an empty body!", 500);
        } else {
            std::cout << "POST METHOD -->" << std::endl;
            return Methods::mPost(req);
        }
    }
    if (method == "DELETE") {
        std::cout << "DELETE METHOD -->" << std::endl;
        return Methods::mDelete(req);
    }
    std::cout << "GET METHOD -->" << std::endl;
    return Methods::mGet(req);
}

