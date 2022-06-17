#!/usr/bin/env python3

from functions import *

body = get_body()
body += "<h1>Вы успешно вышли из системы!</h1>"

main_page = get_head_html('Выход из системы') + body + "</body>\n</html>"

print(get_response().format(server=os.environ['SERVER_NAME'],
                            cont_len=len(main_page.encode('utf-8')),
                            cont_type=os.environ['CONTENT_TYPE']) + "\nSet-Cookie:user=vagiz;Max-age=0" +
      "\r\n\r\n" + main_page + "\r\n\r\n")