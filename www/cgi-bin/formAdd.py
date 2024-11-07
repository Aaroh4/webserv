#!/usr/bin/python3

import cgi
import os

file_path = "www/data/strings.txt"

form = cgi.FieldStorage()

name = form.getvalue('input', '')

with open(file_path, 'a') as file:
    file.write(f"{name}\n")
print("Content-Type: text/html\n")
print(f"Content-Length: {len(name)}\n")
print(f"{name}")