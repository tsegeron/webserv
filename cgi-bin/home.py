#!/usr/local/bin/python3

import os
from functions import *

body = get_body()

body += """<h1>Главная страница</h1>
    """
body += "\t<p>" + os.environ['COOKIE'] + '</p>\n'
for i in os.environ.keys():
    body += "\t<p>" + i + ":" + os.environ[i] + '</p>\n'

main_page = get_head_html('Домашняя страница') + body + "</body>\n</html>"

print(get_response().format(server=os.environ['SERVER_NAME'],
                            cont_len=len(main_page.encode('utf-8')),
                            cont_type=os.environ['CONTENT_TYPE']) +
      "\r\n\r\n" + main_page + "\r\n\r\n")