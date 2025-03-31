/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:41:08 by asalo             #+#    #+#             */
/*   Updated: 2025/03/31 11:37:31 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../incs/StaticHandler.hpp"
#include "../incs/Logger.hpp"
#include "../incs/UploadHandler.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h> //Stat

StaticHandler::StaticHandler() {};

StaticHandler::~StaticHandler() {};

std::string StaticHandler::processRequest(HttpRequest &req) {
    std::string uri = req.getUri();
    std::string method = req.getMethod();

    // std::cout << "Processing Request:" << std::endl;
    // std::cout << "Method: " << method << std::endl;
    // std::cout << "URI: " << uri << std::endl;
    // std::cout << "Body Length: " << req.getBody().size() << std::endl;

    if (method == "POST") {
        if (req.getBody().empty()) {
            Logger::getInstance().logLevel("ERROR", "POST request has an empty body!", 500);
            // std::cerr << "[ERROR] POST request has an empty body!" << std::endl;
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

