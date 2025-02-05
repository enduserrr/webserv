# WEBSERV
* Allowed to use fcntl() with the following flags: (Any other flag is forbidden.)
 * F_SETFL
 * O_NONBLOCK
 * FD_CLOEXEC
* Routes wont be using regexp (reqular expression lang)
* Define a HTTP redirection
* MIME

HTTP redirection is a mechanism used by web servers to instruct a client (like a web browser) to request a different URL than the one originally requested. This is typically done by sending a 3xx HTTP status code (such as 301 for permanent redirection, 302 for temporary redirection, or 307/308 for other variations) along with a "Location" header that specifies the new URL. The client then automatically makes a new request to that URL.


* Methods class finnish
* Router class finnish (calls for correct handler, redirections)
* Delete implementation to CgiHandler
