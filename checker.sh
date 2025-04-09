#!/bin/bash
while true; do
    curl -X POST -d "username=test$(date +%s)" http://localhost:8080/cgi-bin/welcome.php &
done
