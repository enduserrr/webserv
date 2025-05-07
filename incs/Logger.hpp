#ifndef LOGGER_HPP
# define LOGGER_HPP

#include "Global.hpp"
#include "ServerBlock.hpp"

class ServerBlock;

class Logger {
    private:
        std::ofstream                                       _logFile;
        int                                                 _state;
        std::string                                         _defaultErrorPage;
        std::map<int, std::string>                          _errorPages;
        std::map<int, std::string>                          _defaultErrorPages;
        std::map<int, std::pair<std::string, std::string>>  _responses;

        Logger(); // Singleton => private constructor prevents multiple instances
        std::string getCurrentTimestamp() const;
        std::string loadFileContent(const std::string& filePath);

    public:
        ~Logger();
        static Logger& getInstance();

        void        setCustomErrorPage(int code, const std::string &pageContent);
        void        checkErrorPages(ServerBlock &block);
        void        resetErrorPages();
        std::string logLevel(std::string level, const std::string &message, int code);
        std::string getErrorPage(int code);
        std::string getMessage(int code);
        void setLogFile(const std::string &filePath);

};

#endif

