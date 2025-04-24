# WEBSERV

## Html Status Codes
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

* Redirection
 * 301 Moved Permanently
 * 302 Found

## Todo

* DoS protection | OK?
* Text length limit for post | NO!
* Text upload doesn't have white space like space or new line. | OK!
* Delete fail page
* _resourceMap to locate resources correctly (shouldn't set them in constructor => not dynamic)
* remove any hardcoded locations


## NOTE AFTER DOING AN EVAL
* Max header size, url size etc (defined in the config file)
* Serverloop for each server block
* DELETE handling re think
* location for /


## Test
- Proper chunked req
- Spam a lot of clients (in/out success > 99%)
- valgrind --leak-check=full --track-fds=yes ./webserv
- Eval sheet curl commands

### WRK:
wrk -t 2 -c 4 -d 2s http://127.0.0.1:8080/index.html (2 threads, 4 clients, 10 seconds)
wrk -t 4 -c 100 -d 60s http://127.0.0.1:8080/empty.html > wrk_8080_log

### CURL:
- curl -v -X POST http://localhost:8080/uploads \
     -H "Content-Type: application/x-www-form-urlencoded" \
     --data-binary "text=Hello%20world"

- curl -v -X POST http://localhost:8080/uploads \
     -H "Transfer-Encoding: chunked" \
     -F "file=@photo.jpg" \
     --data-binary ""