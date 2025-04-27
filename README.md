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

## TESTING

### Open fd's:
* `valgrind --leak-check=full --track-fds=yes ./webserv`

### Siege:
* siege -b -c 10 -t 10s "http://127.0.0.1:8080/empty.html"
* siege -b -c 10 -t 5s "http://127.0.0.1:8080/cgi-bin/guestbook_display.php"

### Curl:
* curl -v -X POST http://localhost:8080/uploads \
        -H "Content-Type: application/x-www-form-urlencoded" \
        --data-binary "text=Hello%20world"

## Conciderations:
* DELETE fail page (& success?) | OK!
* Log file of server output | OK!
* Logger message for closing any socket | OK!
* Logger message for starting any server loop | OK!
* DELETE/POST/GET to return correct headers etc info | OK! (Method & CgiHandler)
* Multiple server loops to work on the same port | OK!
* Random new line after CTRL+C | OK!
* "Serverloop terminated" message x4 when exiting during child process with CTRL+C | OK!

* Use case for server name (_name) => Add to each server loop so server loop closing message can use it
* Set a file to answer if directory requests (what should it be called in config file)

* Check for open fd's
* Briefs for ServerLoop, CgiHandler & Methods funcs
* Max header(s) & url size limit

## _pollFds
* Fd0: Standard input (stdin).
* Fd1: Standard output (stdout).
* Fd2: Standard error (stderr).

