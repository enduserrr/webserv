/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:35:44 by asalo             #+#    #+#             */
/*   Updated: 2025/04/01 11:09:10 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef METHODS_HPP
# define METHODS_HPP

#include "Global.hpp"
#include "HttpRequest.hpp"
#include "UploadHandler.hpp"
#include "Logger.hpp"
#include "Types.hpp"

class Methods {
    public:
        Methods();
        ~Methods();

        static std::string mGet(HttpRequest &req);
        static std::string mPost(HttpRequest &req);
        static std::string mDelete(HttpRequest &req);
        static std::string generateDirectoryListing(const std::string &directoryPath, const std::string &uri);
};

#endif
