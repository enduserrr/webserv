# WEBSERV
## FIXED:
* Fix "new_" prefix to only apply to duplicates | OK!
* "/" doesn't go to index page (taken as directory listing) | OK!


## Issues
* Setting auto index for locations does not work. Doesn't end up to HttpRequest incase of a set location
* Failed delete doesn't return an error
* Mess

Auto index info could be a map where location is the key & on/off bool is value. Would house any nb of locations, block root included.
