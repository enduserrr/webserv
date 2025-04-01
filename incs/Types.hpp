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
        std::map<std::string, std::string>  _mimeTypes;
        std::vector<std::string>            _contentTypes; //Change to std::list
    public:
        Types();
        ~Types();
        // Types& Types::getInstance() {
        // // Meyers' Singleton:
        // // The 'instance' is created only the *first* time getInstance() is called.
        // // Its initialization is guaranteed to be thread-safe in C++11 and later.
        // // It will be automatically destroyed when the program terminates.
        // static Types instance;
        // return instance;
        // }
        std::string getMimeType(const std::string &extension) const;
        bool isValidMime(const std::string &extension) const;
        bool isValidContent(const std::string &contentType) const;

};

#endif
