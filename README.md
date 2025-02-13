# WEBSERV

## To do:
* autoI & root info pass to HttpReq (fix: HttpParser createRequest modified) | DONE
* Router.cpp -> rid the 'auto' keyword | DONE
* Signal handling (global bool for server loop on off) | DONE
* Integrate readFullRequest function | DONE
* ClientSession class containing the state of each client (requests and responses) | DONE

* Add logic to select & use different ServerBlocks
* Should multiple ServerBlocks be able to listen same ports?
* Signal handling finetuning (cleaning etc, shouldn't log an error after ctr-c?)
* Http page for uploading and deleting uploaded content
* Login Http page that leads to uploading page
* Addind fd tag to HttpParser so requests can be tagged with correct client fd to return the results to
* DDOS safety
* MIME types

