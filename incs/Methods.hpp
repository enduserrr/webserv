/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:35:44 by asalo             #+#    #+#             */
/*   Updated: 2025/03/28 10:44:09 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef METHODS_HPP
# define METHODS_HPP

#include "HttpRequest.hpp"
#include "UploadHandler.hpp"
#include "ErrorHandler.hpp"
#include "Webserver.hpp"
#include "Types.hpp"

class Methods {
    private:

    public:
        Methods();
        ~Methods();

        static std::string mGet(HttpRequest &req);
        static std::string mPost(HttpRequest &req);
        static std::string mDelete(HttpRequest &req);
        static std::string generateDirectoryListing(const std::string &directoryPath, const std::string &uri);
};

#endif
