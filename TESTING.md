# TESTS

## BASIC - SINGLE LOOP | OK!
* Multiple Ports Configuration:
  * curl http://localhost:8080
  * curl http://localhost:8081

* Client Body Size Limit Enforcement: OK!
  * curl -v -X POST http://localhost:8080/uploads \
          -H "Content-Type: application/x-www-form-urlencoded" \
          --data-binary "text=Hello%20world"

* Routing to Different Locations/Directories: OK!
  * curl http://localhost:8080/empty.html
  * curl http://localhost:8080/cgi-bin/guestbook!_display.php

* Custom Error Page Handling (404): OK!
  * curl -i http://localhost:8080/nonexistent_page.txt

* Directory Index File Serving: OK!
  * curl http://localhost:8080/uploads/

* Method Restriction per Route: OK!
  * curl http://localhost:8080/cgi-bin/

* Basic GET Request Handling: OK!
  * curl -X GET http://localhost:8080/index.html

* Basic POST Request Handling: OK!
  * curl -X POST --data "test_data" http://localhost:8080/uploads

* Basic DELETE Request Handling: OK!
  * curl -v -X DELETE "http://127.0.0.1:8080/delete?file=<file_name>"

* Unknown/Invalid HTTP Method Handling: OK!
  * curl -X INVALIDMETHOD http://localhost:8080/

* Directory Listing (Autoindex): OK!
  * curl -X GET http://localhost:8080/uploads/

* HTTP Redirection (301/302)
  * curl -i http://127.0.0.1:8080/redirect


## CGI | OK!
* CGI Execution with GET Method: OK!
  * curl -X GET http://localhost:8080/cgi-bin/guestbook_display.php

* CGI Execution with POST Method: OK!
  *  curl -X POST \
     -d "username=TestUser" \
     -d "message=Hello from curl" \
     http://localhost:8080/cgi-bin/guestbook.php

* CGI Script Relative Path File Access

* CGI Script Timeout/Infinite Loop Handling OK!
  * curl -X GET http://localhost:8080/cgi-bin/infinite.php

* CGI Script Execution Error Handling OK!
  * curl -i http://localhost:8080/cgi-bin/fatal_error.php

## CHUNKED POST REQUEST | OK!
* CHUNKED BINARY FILE: (head -c 100000 /dev/urandom > big.txt) OK!
  * curl -v http://localhost:8080/uploads \
          -H "Transfer-Encoding: chunked" \
          --data-binary "@big.txt"

* CHUNKED TEXT FILE: (yes "this is a line of text" | head -n 5000 > big.txt) OK!
  * curl -v http://localhost:8080/uploads \
          -H "Transfer-Encoding: chunked" \
          --data-binary "@test.txt"

* CHUNKED TEXT/PLAIN: OK!
  * curl -v http://localhost:8080/uploads \
          -H "Content-Type: text/plain" \
          -H "Transfer-Encoding: chunked" \
          --data-binary "hello again from plain text"

* CHUNKED FORM FIELD: OK!
  * curl -v http://localhost:8080/uploads \
          -H "Content-Type: application/x-www-form-urlencoded" \
          -H "Transfer-Encoding: chunked" \
          --data-binary "username=tester&message=chunked%20rocks"

* CHUNKED MULTIPART/FORM-DATA: OK!
  * curl -v http://localhost:8080/uploads \
          -H "Transfer-Encoding: chunked" \
          -F "file=@test.txt"

## MULTI LOOP | OK!
* Multiple Server Names (Hostnames) on Same Port OK!
  * curl --resolve testiservu1.com:8080:127.0.0.1 http://testiservu1.com:8080/
  * curl --resolve testiservu2.com:8081:127.0.0.2 http://testiservu2.com:8081/

## STRESS TEST - SIEGE | OK!
* Stress Test: Basic GET Availability: OK!
  * siege -b -c50 -t30s http://localhost:8080/empty.html

* Stress Test: Server Stability During Continuous Load (Siege)
  * siege -b -c10 http://localhost:8080/empty.html

## OTHER | OK!
* Handling Abrupt Client Disconnect During Upload OK!
  * read_err.py

* Big header: OK!
  * curl -v -H "Host: $(printf 'a%.0s' {1..100000}).testiservu1.com" http://127.0.0.1:8080/empty.html

* Open FD's: OK!
  * valgrind --leak-check=full --track-fds=yes ./webserv