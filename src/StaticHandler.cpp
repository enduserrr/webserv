/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:41:08 by asalo             #+#    #+#             */
/*   Updated: 2025/04/12 18:29:28 by asalo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StaticHandler.hpp"

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

std::string StaticHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string method = req.getMethod();
    
    if (method == "POST" || method == "DELETE") {
        if (req.getBody().empty()) {
            Logger::getInstance().logLevel("ERROR", "POST request has an empty body!", 500);
        } else {
            return Methods::mPost(req);
        }
    }
    // if (method == "DELETE") {
    //     return Methods::mDelete(req);
    // }
    return Methods::mGet(req);
}

