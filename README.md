# WEBSERV

## To do:
- Request parsing to correctly parsing content-type header
- Client count per ip address checker (could count ClientSessions per ip address)
- Max request count in a timeframe and cool down period (ddos safety)
- POST to check for accepted MIME types, logic where uploads folder is created to (server block) | OK!
- Ensuring the server won't get stuck in to method handling | OK!


### HTTP
* Http page for uploading and deleting uploaded content
* Login Http page that leads to uploading page

### Serving the Client
* Moving the responsing and correct client fd to Router?

### Client Permissions & Handling POST and DEL Requests
* POST & DELETE finetuning (should create upload folder to the directory the request was sent for)
* MIME types

### Reporting & Errors
* Log levels info/warning/error to enable accurate reporting


###
- File up doesn't  work
- File ups to take names
