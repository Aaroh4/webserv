#!/usr/bin/python3

import cgi
import os

file_path = "www/data/strings.txt"

print("Content-Type: text/html\n")

form = cgi.FieldStorage()

name = form.getvalue('input', '')

with open(file_path, 'a') as file:
    file.write(f"{name}\n")
print(f"Content-Length: {len(name)}\n")
print(f"{name}")