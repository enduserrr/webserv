# WEBSERV

*"This is when you finally understand why a URL starts with HTTP"*

## Project Overview

The aim of this project was developing relatively simple HTTP/1.1 server in C++. The server is designed to handle client requests, serve static content, process CGI scripts, manage file uploads and deletions, and support GET, POST & DELETE HTTP methods and configurations.

## Table of Contents

1.  [Features](#features)
    *   [Core Functionality](#core-functionality)
    *   [Configuration File](#configuration-file)
    *   [HTTP Methods & Handling](#http-methods--handling)
    *   [CGI (Common Gateway Interface)](#cgi-common-gateway-interface)
2.  [Technical Details](#technical-details)
    *   [I/O Multiplexing](#io-multiplexing)
    *   [HTTP Status Codes Implemented](#html-status-codes-implemented)
    *   [Limitations & Constraints](#limitations--constraints)
    *   [Development Standards](#development-standards)
3.  [Run](#run)
4.  [Team](#team)

## Features

### Core Functionality

*   **Non-Blocking I/O:** Employs non-blocking operations with a single `poll()` instance for efficient, simultaneous handling of multiple clients and server listen sockets.
*   **Content Delivery:** Serves static websites (HTML, CSS, JS, images) and supports dynamic directory listings (autoindex).
*   **Error Handling:** Provides default and configurable custom error pages.
*   **Multi-Port & Resilience:** Listens on multiple ports and is designed for stability under stress, never crashing unexpectedly.
*   **Standards:** Aims for compatibility with standard web browsers and uses NGINX as a reference for HTTP/1.1 behavior.

### Configuration File

Server behavior is defined using a configuration file that enables:

*   **Virtual Servers:** Multiple `server` blocks, each with `host`, `port`, and optional `server_name` (first acts as default for host:port).
*   **Content Management:** `root` directory per server/location, default `index` files, `autoindex` toggles.
*   **Request Control:** `client_max_body_size` limits, custom `error_page` setup.
*   **Routing (Location Blocks):**
    *   Method restrictions (`allow_methods`).
    *   HTTP `return` redirections (301/302).
    *   `upload_store` for file uploads.
    *   CGI handling based on file extensions.

### HTTP Methods & Handling

*   **GET:** Implemented for retrieving resources.
*   **POST:** Implemented for submitting data, (files uploads to the server into a configured directory)
*   **DELETE:** Implemented for deleting resources on the server (e.g., uploaded files).
*   **HTTP Status Codes:** The server responds with appropriate HTTP status codes for all requests.

### CGI (Common Gateway Interface)

*   **CGI Execution:** Supports CGI script execution for dynamic content generation with possible time-outs handled.
*   **GET & POST Methods for CGI:** CGI scripts can be triggered by both GET and POST requests.
*   **Environment Variables:** Essential CGI environment variables are passed to the script (e.g., `REQUEST_METHOD`, `QUERY_STRING`, `SCRIPT_FILENAME`, `CONTENT_LENGTH`, `CONTENT_TYPE`).
*   **Working Directory:** The CGI script is run in the correct directory to allow for relative path file access within the script.
*   **Single CGI Support:** Designed to work with at least one type of CGI interpreter.

## Technical Details

### I/O Multiplexing
* Non-blocking I/O multiplexing with poll():
  * Tell the OS to watch the specified sockets (listening socket for new connections & client sockets)
  * Specify what to listen for on each socket (new connection / incoming data)
  * poll() waits (blocks) until any of the monitored sockets have an event its listening for
  * When poll() returns it tells which sockets are ready and for what event
  * Based on poll() result new client can be accepted, data received from a client with recv or data be sent to a client.

### HTTP Status Codes Implemented
* Error:
  * 400 Bad Request              (The request is malformed or invalid)
  * 403 Forbidden                (The client lacks permission to access the resource)
  * 404 Not Found                (The reguested resource doesn't exist)
  * 405 Method Not Allowed       (The requested method isn't allowed for the resource)
  * 413 Payload Too Large        (The request body size is too large)
  * 429 Too Many Requests        (Exceeded client or per client request limit)
  * 500 Internal Server Error    (Unexpected server failure)
  * 504 Gateway Timeout          (CGI script timed out)
* Success:
  * 200 OK
  * 201 Created
  * 202 Accepted
* Redirection:
  * 301 Moved Permanently
  * 302 Found

### Limitations & Constraints
* As per requirements the program isn't allowed to check `errno` after any `read` or `write` operations. This is of course rather contradictory and highly unusual compared to standard practice for robust error handling. The server relies on return values (e.g., 0 or -1 from `recv`) and `poll` flags for error detection.

* Virtual name based hosting works on the server side (back end). How ever, due to not having sudo permissions modifying "/etc/hosts" or DNS setting isn't possible, and therefore accessing website via a browser using just the server name is not possible without manual client-side resolution (e.g., using `curl --resolve`).

### Development Standards
*   **C++11:** The code is written in C++11 standard.
*   **Compiler Flags:** Compiled with `-Wall -Wextra -Werror`.
*   **Makefile:** A `Makefile` is provided with standard rules: `$(NAME)`, `all`, `clean`, `fclean`, `re` and `run`.

## Run

1.  **Clone the repository:**
    ```bash
    git clone <repository_url> webserv
    cd webserv
    ```
2.  **Compile the project:**
    ```bash
    make
    ```
3.  **Run the server:**
    ```bash
    ./webserv [path_to_configuration_file]
    ```
    If no configuration file is provided, the server may attempt to use a default path (if implemented). Example:
    ```bash
    ./webserv configs/default.conf
    ```

## Team

*   [enduserrr](https://github.com/enduserrr)
*   [eleppala] (https://github.com/eleppala)

*(This project was completed by a team of two, while it is designed to be done by a group of three.)*