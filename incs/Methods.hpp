/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Methods.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/05 11:35:44 by asalo             #+#    #+#             */
/*   Updated: 2025/02/05 12:24:53 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef METHODS_HPP
# define METHODS_HPP

#include "HttpRequest.hpp"
#include "UploadHandler.hpp"
#include "ErrorHandler.hpp"
#include <string>

class Methods {
    private:

    public:
        Methods();
        ~Methods();

        static std::string mGet(HttpRequest &req);
        static std::string mPost(HttpRequest &req);
        static std::string mDelete(HttpRequest &req);
};

#endif
