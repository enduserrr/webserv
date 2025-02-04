/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StaticHandler.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 11:40:49 by asalo             #+#    #+#             */
/*   Updated: 2025/02/04 11:14:20 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef STATICHANDLER_HPP
#define STATICHANDLER_HPP

#include "HttpRequest.hpp"
#include <string>

class StaticHandler {
    public:
        StaticHandler();
        ~StaticHandler();

        std::string processRequest(HttpRequest &req);
};

#endif
