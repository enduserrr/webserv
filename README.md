# WEBSERV

## AGENDA:

* Error Pages & Log Levels:
 * Which file returns what (which are relevant to what client receives)
 * Info/Warning/Error levels
 * Custom error pages? (& defaults)


* Location Vars:
 * Take location info from HttpRequest

* mDELETE:
 * DELETE to work only in the allowed folder
 * Delete button in listing.html doesn't work (Methods.cpp)

* POST:
 * "new_" fix (currently adds to all)
 * Size limit for ups doesn't work
 * Error return doesn't work

## FIXED:
* Dup file names: Adds  "new_" infront of the filename if a file with the same name exists | OK!
* Unified Error pages (.html) with corrected error messages. | OK!
* Removing client properly after handling request | OK!
* DELETE method | OK!
* Multipart upload | OK!
* Delete | OK!


## SUNDAY:
* Fix "new_" prefix to only apply on duplicates

## Issues
* Setting auto index for locations does not work. Issue in setting locations into server block or
looking for matching location in HttpParser (create request)
* "/" doesn't go to index page (taken as directory listing)
