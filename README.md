# WEBSERV

## FIXED:
* Fix "new_" prefix to only apply to duplicates | OK!
* "/" doesn't go to index page (taken as directory listing) | OK!
* Auto Index per Location | OK!
* ConfParser udpates location root correctly | OK!

## Issues
* Delete & dir listing fail not returning a proper error
* Library mess

## TO DO LIST:
* Delete method error handling & replies
* CGI tests (more tests over all)

## Error State Plan

* ClientSession:
 * Add '_state' attribute + setState() & getState() funcs
 * Add ErrorHandler functions

* ErrorHandler:
 * Remove ErrorHandler all together

* ServerLoop:
 * Remove ErrorHandler usage from setupServerSockets(), replace with console error messages
 * Add logic to check '_state' (ClientSession) and send matching error pages and set back to 0 after
 * Remove needless code from handleClientRequest()

* HttpParser:
 * Add a way to access ClientSessions setState() & getState() functions

* HttpRequest:
 * Add a way to access ClientSessions setState() & getState() functions

* Router, Methods, Cgi, Static
 * Replace ErrorHandler usage with ClientSession setState()
