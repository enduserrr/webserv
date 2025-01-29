/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorHandler.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/26 16:21:24 by asalo             #+#    #+#             */
/*   Updated: 2025/01/26 16:21:26 by asalo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORHANDLER_HPP
# define ERRORHANDLER_HPP

# include <string>
# include <map>
# include <iostream>
# include <fstream>

class ErrorHandler {
    private:
        std::map<int, std::string> _errorPages;
        std::map<int, std::string> _defaultErrorPages;

    public:
        ErrorHandler();
        ~ErrorHandler();

        void    logError(const std::string &message);
        std::string getErrorPage(int status);

        void setErrorPage(int status, const std::string &path); // Get an error page (custom if set, default if not)
};


#endif
