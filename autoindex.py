#!/usr/local/bin/python3

import html
import os

from functions import *


os.environ['FILENAME'] = '../pages'
os.environ['LOGIN'] = '123'
body = "<body>"

path = os.environ.get('FILENAME')
if os.path.isdir(path):
    for i in os.listdir(path):
        body += f"\t<a href=\"/{path}/{i}\">{path}.{i}</a><br/>\n"
elif os.path.isfile(path):
    with open(path, 'r') as f:
        for i in f:
            body += html.escape(i) + "<br />\n"

main_page = get_head_html('autoindex') + body + "</body>\n</html>"

print("""HTTP/1.1 200 OK\r
        Server: {server}\r
        Content=length: {cont_len}\r
        Content-Type: {cont_type}\r\n\r\n""".format(server=os.environ['SERVER_NAME'],
                                                    cont_type=os.environ['CONTENT_TYPE'],
                                                    cont_len=len(main_page.encode('utf-8')))
      + main_page + '\r\n\r\n')