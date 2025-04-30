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

## About fd's
* Fd0: Standard input (stdin)
* Fd1: Standard output (stdout)
* Fd2: Standard error (stderr)

## About virtual name based hosting
Virtual name based hosting works on the server side (back end). How ever due to not having sudo permissions and being unable to modify "/etc/hosts" or DNS setting, accessing testing webstie via a browser using just the server name is not possible.

## TESTS
### Curl:
* GET:
  * curl http://127.0.0.1:8080/empty.html
* POST:
  * curl -v -X POST http://localhost:8080/uploads \
          -H "Content-Type: application/x-www-form-urlencoded" \
          --data-binary "text=Hello%20world"

* CHUNKED BINARY FILE: (head -c 100000 /dev/urandom > big.txt)
  * curl -v http://localhost:8080/uploads \
          -H "Transfer-Encoding: chunked" \
          --data-binary "@big.txt"

* CHUNKED TEXT FILE: (yes "this is a line of text" | head -n 5000 > big.txt)
  * curl -v http://localhost:8080/uploads \
          -H "Transfer-Encoding: chunked" \
          --data-binary "@test.txt"

* CHUNKED TEXT/PLAIN:
  * curl -v http://localhost:8080/uploads \
          -H "Content-Type: text/plain" \
          -H "Transfer-Encoding: chunked" \
          --data-binary "hello again from plain text"

* CHUNKED FORM FIELD:
  * curl -v http://localhost:8080/uploads \
          -H "Content-Type: application/x-www-form-urlencoded" \
          -H "Transfer-Encoding: chunked" \
          --data-binary "username=tester&message=chunked%20rocks"

* CHUNKED MULTIPART/FORM-DATA:
  * curl -v http://localhost:8080/uploads \
          -H "Transfer-Encoding: chunked" \
          -F "file=@test.txt"

* VIRTUAL NAME BASED HOSTING:
  * curl --resolve testiservu1.com:8080:127.0.0.1 http://testiservu1.com:8080
  * curl --resolve testiservu1.com:8080:127.0.0.1 http://testiservu1.com:8080/cgi-bin/guestbook_display.php

* BIG HEADER:
  * curl -v -H "Host: $(printf 'a%.0s' {1..100000}).testiservu1.com" http://127.0.0.1:8080/empty.html


### Fd's:
(0, 1 & 2 should be left open)
* valgrind --leak-check=full --track-fds=yes ./webserv

### Siege:
Stress testing with multiple clients
* siege -b -c 10 -t 10s "http://127.0.0.1:8080/empty.html"
* siege -b -c 10 -t 5s "http://127.0.0.1:8080/cgi-bin/guestbook_display.php"

## Conciderations:
* DELETE fail page (& success?) | OK!
* Log file of server output | OK!
* Logger message for closing any socket | OK!
* Logger message for starting any server loop | OK!
* DELETE/POST/GET to return correct headers etc info | OK! (Method & CgiHandler)
* Multiple server loops to work on the same port | OK!
* Random new line after CTRL+C | OK!
* "Serverloop terminated" message x4 when exiting during child process with CTRL+C | OK!
* Check for open fd's |OK!
* Max header(s) & url size limit (can handle atleast 100K chars but should be limited imo) OK!
* Name based virtual hosting | OK!
* Set a file to answer if directory requests | OK!
* Briefs for ServerLoop, CgiHandler & Methods funcs | OK!



