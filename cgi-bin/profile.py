#!/usr/bin/env python3
from functions import *

body = get_body()

body += """<h1> Мой профиль</h1>
    <form action="/form" method="post">
        <p>Имя <input type="text" name="firstname"></p>
        <p>Фамилия <input type="text" name="lastname"></p>
        <input type="submit" value="Еблуша">
    </form>
    <form action="/upload" method="post" blank="_target" enctype="multipart/form-data">
        <p><input type="file" name="file"></p>
        <input type="submit" value="Upload">
    </form>
    """

main_page = get_head_html('Мой профиль') + body + "</body>\n</html>"

print(get_response().format(server=os.environ['SERVER_NAME'],
                            cont_len=len(main_page.encode('utf-8')),
                            cont_type=os.environ['CONTENT_TYPE']) + "\r\n\r\n" + main_page + "\r\n\r\n")
