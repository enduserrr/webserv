/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:40:49 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:09:10 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef STATICHANDLER_HPP
#define STATICHANDLER_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "Methods.hpp"

class StaticHandler {
    public:
        StaticHandler();
        ~StaticHandler();

        std::string processRequest(HttpRequest &req);
};

#endif
