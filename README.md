# WEBSERV

## To do:

### Multiple Clients & Blocks
* startServer & handleClientRequest to correctly check for client connections on sockets | OK!
* Add logic to support multiple ServerBlocks | OK!
* Multiple ServerBlocks able to listen same ports? - NO | OK!

### Exiting, Cleaning & Signals
* Signal handling finetuning (cleaning etc, shouldn't log an error after ctr-c?)

### HTTP
* Http page for uploading and deleting uploaded content
* Login Http page that leads to uploading page

### Serving the Client
* Moving the responsing and correct client fd to Router?

### Perserverance
* DDOS safety
* Any jamming

### Client Permissions & Handling POST and DEL Requests
* POST & DELETE finetuning (should create upload folder to the directory the request was sent for)
* MIME types

### Reporting & Errors
* Log levels info/warning/error to enable accurate reporting
