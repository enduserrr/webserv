/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Types.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/08 09:53:12 by asalo             #+#    #+#             */
/*   Updated: 2025/02/20 10:50:44 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef TYPES_HPP
# define TYPES_HPP

#include "Global.hpp"

class Types {
    private:
        Types();
        std::map<std::string, std::string>  _mimeTypes;
        std::vector<std::string>            _contentTypes; //Change to std::list
    public:
        static Types& getInstance();
        // Types();
        ~Types();

        std::string getMimeType(const std::string &extension) const;
        bool isValidMime(const std::string &extension) const;
        bool isValidContent(const std::string &contentType) const;

};

#endif
