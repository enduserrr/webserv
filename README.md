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

## Testing

### FD'S:
(0, 1 & 2 should be left open)
* valgrind --leak-check=full --track-fds=yes ./webserv

### SIEGE:
Stress testing with multiple clients
* siege -b -c 10 -t 10s "http://127.0.0.1:8080/empty.html"
* siege -b -c 10 -t 5s "http://127.0.0.1:8080/cgi-bin/guestbook_display.php"

### CURL:
* Simple GET:
  * curl http://127.0.0.1:8080/empty.html

* Simple POST:
  * curl -v -X POST http://localhost:8080/uploads \
        -H "Content-Type: application/x-www-form-urlencoded" \
        --data-binary "text=Hello%20world"

* Name based hosting:
  * curl --resolve testiservu1.com:8080:127.0.0.1 http://testiservu1.com:8080

* Excessive header size:
  * curl -v -H "Host: $(printf 'a%.0s' {1..100000}).testiservu1.com" http://127.0.0.1:8080/empty.html

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

* Briefs for ServerLoop, CgiHandler & Methods funcs

* Set a file to answer if directory requests (what should it be called in config file)
* Logger message after setting custom error page

* Name based virtual hosting | OK! ()
  * setupServerSockets
  * acceptNewConnection
  * handleClientRequest




