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
