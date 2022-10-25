#!/usr/bin/env python3
from functions import *

if not get_auth():
    with open("error_pages/403.html", 'r') as file:
        main_page = file.read()
        print(get_response().format(status="403 Forbidden",
                                    server=os.environ['SERVER_NAME'],
                                    cont_len=len(main_page.encode('utf-8')),
                                    cont_type=os.environ['CONTENT_TYPE']) + "\r\n\r\n" + main_page, end='')
else:
    conn = sqlite3.connect("cgi-bin/web.db")
    cur = conn.cursor()
    login = os.environ.get('LOGIN')
    cur.execute(f"SELECT users.id, users.auth_key FROM users WHERE login = '{login}';")
    res = cur.fetchone()
    id = res[0]
    auth_key = res[1]
    os.environ.pop('LOGIN')
    body = get_body('Выход')
    if auth_key is None:
        body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
  <strong>Вы еще не вошли в систему </strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>"""
        main_page = get_head_html('Ошибка') + body + "</body>\n</html>"
    else:
        cur.execute(f"""UPDATE users SET auth_key = NULL WHERE id = {id};""")
        conn.commit()
        body += """<div class="alert alert-success alert-dismissible fade show" role="alert">
  <strong>Успешно вышли из системы </strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>"""
        main_page = get_head_html('Выход из системы') + body + "</body>\n</html>"

    print(get_response().format(status="200 OK",
                                server=os.environ['SERVER_NAME'],
                                cont_len=len(main_page.encode('utf-8')),
                                cont_type=os.environ['CONTENT_TYPE']) + f"\nSet-Cookie:user={login};Max-age=0" +
          "\r\n\r\n" + main_page, end='')
