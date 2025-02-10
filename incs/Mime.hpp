/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mime.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 09:53:12 by asalo             #+#    #+#             */
/*   Updated: 2025/02/08 10:39:14 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef MIME_HPP
# define MIME_HPP

#include "Libs.hpp"

class Mime {
    private:
        std::map<std::string, std::string> _types;

    public:
        Mime();
        ~Mime();

        std::string getMimeType(const std::string &extension) const;
};

#endif
