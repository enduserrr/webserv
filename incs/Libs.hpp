#ifndef LIBS_HPP
# define LIBS_HPP

/**
 * @brief   Std lib header are typically self-contained and stable.
 *          => Transitive include through Libs.hpp works.
 *
 *          User-defined header need to be directly included to ensure
 *          all their attributes are forwardable instead of only the parts
 *          that have already been declared.
 */

#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <poll.h>
#include <map>
#include <ctime>
#include <functional> //std::function
#include <dirent.h> // For opendir, readdir, closedir
// #include <atomic>

#endif
