# TESTS

## Curl:
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

## Fd's:
(0, 1 & 2 should be left open)
* valgrind --leak-check=full --track-fds=yes ./webserv

## Siege:
Stress testing with multiple clients
* siege -b -c 10 -t 10s "http://127.0.0.1:8080/empty.html"
* siege -b -c 10 -t 5s "http://127.0.0.1:8080/cgi-bin/guestbook_display.php"