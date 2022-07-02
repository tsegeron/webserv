#!/usr/local/bin/python3
# import cgi
from functions import *

# form = cgi.FieldStorage()

query = os.environ['QUERY_STRING']
vals = {}
for line in query.split('&'):
    val = line.split('=')
    vals[val[0]] = val[1]
firstname = vals['firstname']
lastname = vals['lastname']

body = get_body()
body += """<h2>Hello {f}, {l}</h2>""".format(f=firstname, l=lastname)

main_page = get_head_html('Форма') + body + "</body>\n</html>"

print(get_response().format(server=os.environ['SERVER_NAME'],
                            cont_type=os.environ['CONTENT_TYPE'],
                            cont_len=len(main_page.encode('utf-8'))) + "\r\n\r\n" + main_page, end='')
