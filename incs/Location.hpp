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
    std::string                 _location;
    std::vector<std::string>    _allowedMethods;
public:
    Location();
    ~Location();
    // Location(const Location &other);
    // Location& operator=(const Location &other);

    // Getters and Setters
    // int getValue() const;
    // void setValue(int value);

    // Member functions
    // void display() const;
};

#endif // LOCATION_HPP
