#!/usr/bin/env python3
import hashlib
from functions import *
import urllib.parse


def check_form(login, password):
    conn = sqlite3.connect("cgi-bin/web.db")
    cur = conn.cursor()
    salt = os.urandom(32)
    password = hashlib.pbkdf2_hmac(
        'sha256', password.encode('utf-8'), salt, 100000)
    cur.execute(f"SELECT users.login FROM users WHERE users.login='{login}'")
    result = cur.fetchone()
    if result is None:
        cur.execute("""INSERT INTO users(login,password,salt) 
            VALUES
            (?, ?, ?);""", (login, password, salt))
        conn.commit()
        return True
    return False

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
                    <label for="password1" class="form-label">Пароль</label>
                    <input type="password" class="form-control" id="password1" name="password1">
                  </div>
                  <div class="mb-3">
                    <label for="password2" class="form-label">Подтверждение пароля</label>
                    <input type="password" class="form-control" id="password2" name="password2">
                  </div>
                  <button type="submit" class="btn btn-primary">Зарегистрироваться</button>
                </form>
          </div>
        </div>
    </div>
"""
if get_auth():
    body = get_body('Регистрация')
    body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
  <strong>Вы уже пошли в систему. Выйдите, чтобы зарегистрироваться </strong>
  <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
</div>"""
    main_page = get_head_html('Ошибка') + body + "</body></html>"
else:
    body = get_body('Регистрация')
    if os.environ['REQUEST_METHOD'] == 'GET':
        body += form_text
        main_page = get_head_html('Регистрация') + body + "</body>\n</html>"

    else:
        data = os.environ['QUERY_STRING'].split('&')
        dicti = dict()
        for i in data:
            i = i.split('=')
            dicti[i[0]] = i[1]
        login = urllib.parse.unquote_plus(dicti.get('login')) if dicti.get('login') else None
        password1 = urllib.parse.unquote_plus(dicti.get('password1')) if dicti.get('password1') else None
        password2 = urllib.parse.unquote_plus(dicti.get('password2')) if dicti.get('password2') else None
        if not all([login, password1, password2]) or len(data) != 3:
            body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
      <strong>Некорректные данные формы</strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>"""
            body += form_text
            main_page = get_head_html('Регистрация') + body + "</body>\n</html>"
        elif password1 != password2:
            body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
      <strong>Пароли не совпадают </strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>"""
            body += form_text.format(value=f"value=\"{login}\"")
            main_page = get_head_html('Регистрация') + body + "</body>\n</html>"
        elif not check_form(login, password1):
            body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
      <strong>Пользователь с таким логином уже существует </strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>"""
            body += form_text.format(value=f"value=\"{login}\"")
            main_page = get_head_html('Ошибка пользователь не создан!') + body + "</body>\n</html>"
        else:
            body += """<div class="alert alert-success alert-dismissible fade show" role="alert">
      <strong>Вы успешно создали учетную запись</strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>"""
            main_page = get_head_html('Пользователь успешно создан') + body + "</body>\n</html>"

print(get_response().format(status="200 OK",
                            server=os.environ['SERVER_NAME'],
                            cont_type=os.environ['CONTENT_TYPE'],
                            cont_len=len(main_page.encode('utf-8'))) +
      "\r\n\r\n" + main_page, end='')
