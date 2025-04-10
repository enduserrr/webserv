/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Router.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:02:44 by asalo             #+#    #+#             */
/*   Updated: 2025/04/10 21:32:38 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "CgiHandler.hpp"
#include "StaticHandler.hpp"
#include "Logger.hpp"

class Router {
private:
    Router();

public:
    static Router& getInstance();
    ~Router();

    std::string routeRequest(HttpRequest &req);
};

#endif

