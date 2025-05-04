# WEBSERV

## HTML Status Codes
* Error:
  * 400 Bad Request              (The request is malformed or invalid)
  * 403 Forbidden                (The client lacks permission to access the resource)
  * 404 Not Found                (The reguested resource doesn't exist)
  * 405 Method Not Allowed       (The requested method isn't allowed for the resource)
  * 413 Payload Too Large        (The request body size is too large)
  * 429 Too Many Requests        (Exceeded client or per client request limit)
  * 500 Internal Server Error    (Unexpected server failure)
* Success:
  * 200 OK
  * 201 Created
  * 202 Accepted
* Redirection:
  * 301 Moved Permanently
  * 302 Found

## About virtual name based hosting
Virtual name based hosting works on the server side (back end). How ever, due to not having sudo permissions modifying "/etc/hosts" or DNS setting isn't possible, and therefore accessing website via a browser using just the server name is not possible.

# About restrictions
As per requirements the program isn't allowed to check errno after any read or write operations. This is of course rather contradictory and highly unusual compared to standard practice for robust error handling.

## Last fixes:
* CGI timeout in child process