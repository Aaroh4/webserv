#!/usr/bin/env python3

import cgi
import cgitb
cgitb.enable()  # Enable error logging for debugging

print("Content-type: text/html\n")  # HTTP header with an extra newline

print("<html><body style='text-align:center;'>")  # Corrected "html" tag

form = cgi.FieldStorage()  # Parse form data

# Check if 'name' parameter is present and retrieve it
if "input" in form:
    name = form.getvalue("input")
    print(f"<h1 style='color: green;'>Hello, {name}!</h1>")
else:
    print("<h1 style='color: red;'>Hello, guest!</h1>")  # Default message if no input

print("</body></html>")