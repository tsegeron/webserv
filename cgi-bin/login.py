#!/usr/bin/env python3
from functions import *
import hashlib
import urllib.parse


cookie = ""

if get_auth():
    body = get_body('Вход')
    body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
  <strong>Вы уже пошли в систему </strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>"""
    main_page = get_head_html('Ошибка') + body + "</body></html>"

else:
    body = get_body('Вход')
    form_text = """
    <div class="container-fluid d-flex h-100 justify-content-center align-items-center p-0">
        <div class="row bg-white shadow-sm">
           <div class="col border rounded p-4">
            <h3 class="text-center mb-4">Вход</h3>
                <form method="post">
                  <div class="mb-3">
                    <label for="login" class="form-label">Логин</label>
                    <input type="text" class="form-control" name="login" aria-describedby="login_help" id="login" {value}>
                    <div id="login_help" class="form-text">Поддерживается латиница и кириллица</div>
                  </div>
                  <div class="mb-3">
                    <label for="password" class="form-label">Пароль</label>
                    <input type="password" class="form-control" id="password" name="password">
                  </div>
                  <button type="submit" class="btn btn-primary">Войти</button>
                </form>
          </div>
        </div>
    </div>
"""
    if os.environ['REQUEST_METHOD'] == 'GET':
        body += form_text
        main_page = get_head_html('Вход в систему') + body + "</body>\n</html>"
    else:
        data = os.environ['QUERY_STRING'].split('&')
        dicti = dict()
        for i in data:
            i = i.split('=')
            dicti[i[0]] = i[1]
        login = urllib.parse.unquote_plus(dicti.get('login'))
        password = urllib.parse.unquote_plus(dicti.get('password'))
        if not all([login, password]) or len(data) != 2:
            body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
  <strong>Некорректные данные</strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>""" + form_text
            main_page = get_head_html('Вход в систему') + body + "</body>\n</html>"
        else:
            conn = sqlite3.connect("cgi-bin/web.db")
            cur = conn.cursor()
            cur.execute(f"SELECT users.password, users.id, users.auth_key, users.salt FROM users WHERE users.login = '{login}';")
            res = cur.fetchone()
            if res is None:
                body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
  <strong>Данного логина не сущетвует!</strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>""" + form_text
                main_page = get_head_html('Вход в систему') + body + "</body>\n</html>"
            else:
                user_pass = res[0]
                user_id = res[1]
                user_salt = res[3]
                password = hashlib.pbkdf2_hmac('sha256',
                                               password.encode('utf-8'),
                                               user_salt, 100000)
                if user_pass == password:
                    auth_key = generate_random_string()
                    cur.execute(f"""UPDATE users
                        SET auth_key='{auth_key}' 
                        WHERE id = {user_id};""")
                    conn.commit()
                    os.environ['LOGIN'] = login
                    body = get_body('Вход') + """<div class="alert alert-success alert-dismissible fade show" role="alert">
  <strong>Вы успешно вошли в систему</strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>"""
                    main_page = get_head_html('Успешно') + body + "</body>\n</html>"
                    cookie += f"\nSet-Cookie:login={login};\r\nSet-Cookie:auth-key={auth_key}"
                else:
                    body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
  <strong>Неверный пароль! </strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>""" + form_text.format(value=f"value=\"{login}\"")
                    main_page = get_head_html('Неудача') + body + "</body>\n</html>"

print(get_response().format(status="200 OK",
                            server=os.environ['SERVER_NAME'],
                            cont_type=os.environ['CONTENT_TYPE'],
                            cont_len=len(main_page.encode('utf-8'))) + cookie +
      "\r\n\r\n" + main_page, end='')
