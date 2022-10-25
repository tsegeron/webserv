#!/usr/bin/env python3
import html
from functions import *
import urllib.parse

get_auth()

body = get_body('Новая запись')
data = os.environ['QUERY_STRING'].split('&')
dicti = dict()
for i in data:
    i = i.split('=')
    dicti[i[0]] = i[1]
img_path = urllib.parse.unquote_plus(dicti.get('img_path')) if dicti.get('img_path') else None
description = urllib.parse.unquote_plus(dicti.get('description')) if dicti.get('description') else None
if not all([img_path, description]) or len(data) != 2 or not check_uniq(img_path):
    body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
      <strong>Некорректные данные формы </strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>"""
    main_page = get_head_html('Ошибка публикации') + body + "</body>\n</html>"
else:
    with sqlite3.connect("cgi-bin/web.db") as conn:
        cur = conn.cursor()
        cur.execute(f"SELECT users.id FROM users WHERE login = '{os.environ.get('LOGIN')}';")
        result = cur.fetchone()
        author = result[0]
        cur.execute(f"""INSERT INTO posts(img_path, description, author) 
                    VALUES 
                    (?, ?, ?);""", (img_path, description, author))
        conn.commit()
    body += """<div class="alert alert-success alert-dismissible fade show" role="alert">
      <strong> Пост успешно добавлен! </strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>"""
    main_page = get_head_html('Пост опубликован') + body + "</body>\n</html>"
print(get_response().format(status="200 OK",
                            server=os.environ['SERVER_NAME'],
                            cont_len=len(main_page.encode('utf-8')),
                            cont_type=os.environ['CONTENT_TYPE']) +
      "\r\n\r\n" + main_page, end='')