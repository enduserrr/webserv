/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:35 by eleppala          #+#    #+#             */
/*   Updated: 2025/02/06 08:59:44 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>

class Location {
private:
    std::string                 _path;
    std::vector<std::string>    _allowedMethods;
    // bool                        _dirListing;
    // std::string                 _root;

public:
    Location();
    ~Location();

    // Location(const Location &other);
    // Location& operator=(const Location &other);

    std::string setPath();
    bool setPath(const std::string& path);
    bool addAllowedMethod(const std::string& method);


    std::string getPath() const;
    std::vector<std::string> getAllowedMethods() const;

    // std::string getRoot() {
    //     return _root;
    // }


    // Member functions


    // void display() const;
};

#endif
