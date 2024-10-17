# Webserv - Custom HTTP Server

This project involves creating an HTTP server in C++98, supporting GET, POST, and DELETE methods, serving static websites, handling file uploads, and executing CGI scripts. The server uses non-blocking I/O and can handle multiple clients. Configuration is similar to NGINX, allowing customization of ports, routes, error pages, and more.


Key Features:

Handles HTTP 1.1 requests and responses
Implements non-blocking I/O using poll() or equivalent
Configurable routing, error pages, and file handling
Supports CGI execution (e.g., PHP, Python)
Multi-port listening and file uploads
