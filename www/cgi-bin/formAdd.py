print("Content-type: text/html\n\n")

import cgi
import cgitb
cgitb.enable()

form = cgi.FieldStorage()
if "name" not in form:
    print("<H1>Error</H1>")
    print("Please fill in the name and addr fields.")
    return
print("<p>List of somethings:", form["input"].value)