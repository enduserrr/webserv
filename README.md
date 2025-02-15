# WEBSERV

## To do:

### Multiple Clients & Blocks
* Modify startServer & handleClientRequest to correctly check for client connections on sockets
* Add logic to select & use different ServerBlocks
* Should multiple ServerBlocks be able to listen same ports?

### Exiting, Cleaning & Signals
* Signal handling finetuning (cleaning etc, shouldn't log an error after ctr-c?)

### HTTP
* Http page for uploading and deleting uploaded content
* Login Http page that leads to uploading page

### Serving the Right Client
* Addind fd tag to HttpParser so requests can be tagged with correct client fd to return the results to

### Perserverance
* DDOS safety

### Client Permissions & Handling POST and DEL Requests
* POST & DELETE finetuning (should create upload folder to the directory the request was sent for)
* MIME types
