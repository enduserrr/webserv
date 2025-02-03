/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eleppala <eleppala@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/28 18:44:35 by eleppala          #+#    #+#             */
/*   Updated: 2025/01/28 18:44:37 by eleppala         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <vector>

class Location {
private: 
    std::string                 _path; 
    std::vector<std::string>    _allowedMethods;

public:
    Location();
    ~Location();

    // Location(const Location &other);
    // Location& operator=(const Location &other);
    

    bool setPath(const std::string& path);
    bool addAllowedMethod(const std::string& method);

    
    std::string getPath() const;
    std::vector<std::string> getAllowedMethods() const;


    // Member functions


    // void display() const;
};

#endif 
