import cgi
import cgitb
cgitb.enable()

print("Content-type: text/html\n\n")
print("<hmtl><body style='text-align:center;'>")
form = cgi.FieldStorage()
if form.getvalue("name"):
	name = form.getvalue("name")
    print("<h1 style='color: green;'> Hello, " + name + "!</h1>")
print("</body></html>")