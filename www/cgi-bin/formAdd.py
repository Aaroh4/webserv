#!/usr/bin/env python3
import cgi
import os

# Define the file path
file_path = "../data/strings.txt"

# Ensure the directory exists
os.makedirs(os.path.dirname(file_path), exist_ok=True)

# Get form data
form = cgi.FieldStorage()
name = form.getvalue('input', '')
timeout = os.environ.get("TIMEOUT", "10")
# Open the file in append mode, create it if it doesn't exist
with open(file_path, 'a') as file:
    file.write(f"{name}\n")

# Prepare the HTTP response
response_content = f"""<!DOCTYPE html>
<html>
<body>
<p>Added {name}!</p>
</body>
</html>"""

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print(f"Content-Length: {len(response_content)}")
print("Connection: Keep-Alive")
print(f"Keep-Alive: timeout={timeout}, max=100")
print()
print(response_content)
