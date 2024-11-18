#!/usr/bin/python3

#!/usr/bin/env python3
import cgi
import os


file_path = "www/data/strings.txt"

form = cgi.FieldStorage()
name = form.getvalue('input', '')
timeout = os.environ.get("TIMEOUT", "10")
with open(file_path, 'a') as file:
    file.write(f"{name}\n")

response_content = f"<!DOCTYPE html>\n<html>\n<body>\n<p>Added {name}!</p>\n</body>\n</html>"

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print(f"Content-Length: {len(response_content)}")
print("Connection: Keep-Alive")
print(f"Keep-Alive: timeout={timeout}, max=100")
print()
print(response_content)
