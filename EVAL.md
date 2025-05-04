# EVAL SHEET TESTS

## BASIC - SINGLE LOOP
* Multiple Ports Configuration:
  * curl http://localhost:8080
  * curl http://localhost:8081

* Client Body Size Limit Enforcement: OK
  * curl -v -X POST http://localhost:8080/uploads \
          -H "Content-Type: application/x-www-form-urlencoded" \
          --data-binary "text=Hello%20world"

* Routing to Different Locations/Directories: OK
  * curl http://localhost:8080/empty.html
  * curl http://localhost:8080/cgi-bin/guestbook_display.php

* Custom Error Page Handling (404): OK
  * curl -i http://localhost:8080/nonexistent_page.txt

* Directory Index File Serving: OK
  * curl http://localhost:8080/uploads/

* Method Restriction per Route: OK
  * curl http://localhost:8080/cgi-bin/

* Basic GET Request Handling: OK
  * curl -X GET http://localhost:8080/index.html

* Basic POST Request Handling: OK
  * curl -X POST --data "test_data" http://localhost:8080/uploads

* Basic DELETE Request Handling: OK
  * curl -v -X DELETE "http://127.0.0.1:8080/delete?file=<file_name>"

* Unknown/Invalid HTTP Method Handling: OK
  * curl -X INVALIDMETHOD http://localhost:8080/

* Directory Listing (Autoindex): OK
  * curl -X GET http://localhost:8080/uploads/

* HTTP Redirection (301/302)
  * <fill_a_test_here>

* Prevent Binding Same Port Multiple Times
 * <IS_THIS_REAL>

## CGI
* CGI Execution with GET Method: OK
  * curl -X GET http://localhost:8080/cgi-bin/guestbook_display.php

* CGI Execution with POST Method: OK
  *  curl -X POST \
     -d "username=TestUser" \
     -d "message=Hello from curl" \
     http://localhost:8080/cgi-bin/guestbook.php

* CGI Script Relative Path File Access

* CGI Script Timeout/Infinite Loop Handling
  * curl --max-time 5 http://localhost:8080/cgi-bin/infinite.php

* CGI Script Execution Error Handling (meaby smth else?)
  * curl -i http://localhost:8080/cgi-bin/fatal_error.php

## CHUNKED POST REQUEST
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


## MULTI LOOP
* Multiple Server Names (Hostnames) on Same Port
  * curl --resolve testiservu1.com:8080:127.0.0.1 http://testiservu1.com:8080/
  * curl --resolve testiservu2.com:8080:127.0.0.1 http://testiservu2.com:8081/

## STRESS TEST - SIEGE
* Stress Test: Basic GET Availability (Siege)
  * siege -b -c50 -t30s http://localhost:8080/empty.html

* Stress Test: Hanging Connection Check (Siege + Monitoring)
  * ss -tnp | grep ':8080' | grep '<PID_of_webserv>' or netstat -tnp | grep webserv

* Stress Test: Server Stability During Continuous Load (Siege)
  * siege -b -c10 http://localhost:8080/empty.html

## OTHER
* Handling Abrupt Client Disconnect During Upload
  * read_err.py

* Big header:
  * curl -v -H "Host: $(printf 'a%.0s' {1..100000}).testiservu1.com" http://127.0.0.1:8080/empty.html

* Open FD's:
  * valgrind --leak-check=full --track-fds=yes ./webserv


## Conciderations
* Port issues: prevent connecting to same port multiple times???

