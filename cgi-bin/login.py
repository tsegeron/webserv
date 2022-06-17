#!/usr/bin/env python3

import cgi
from functions import *


body = get_body()
if os.environ['REQUEST_METHOD'] == 'GET':
    body += """<form action="/log_in" method="post">
        <p><input type="text" name="login" required minlength="8" maxlength="16" placeholder="Login" autocomplete="false"></p>
        <p><input type="password" name="password" required minlength="8" maxlength="16" placeholder="Password""></p>
        <input type="submit" value="Войти">
    </form>"""
    main_page = get_head_html('Вход в систему') + body + "</body>\n</html>"
else:
    form = cgi.FieldStorage()

    login = form.getvalue('login')
    password = form.getvalue('password')

    if login in get_logins() and password in get_passwords() and get_passwords()[get_logins().index(login)] == password:
        body += "<h2>Вы успешно вошли в систему! {l}, {p}</h2>\n".format(l=login, p=password)
        main_page = get_head_html('Успешно') + body + "</body>\n</html>"
    else:
        if login is None:
            login = "Логин"
        if password is None:
            password = "Пароль"
        body += """<h2>Неудача {l}, {p}</h2>""".format(l=login, p=password)
        body += """<form action="/log_in" method=get>
        <p><input type="text" name="login" required minlength="8" maxlength="16" placeholder="Login"></p>
        <p><input type="password" name="password" required minlength="8" maxlength="16" placeholder="Password""></p>
        <input type="submit" value="Войти">
    </form>"""
        main_page = get_head_html('Неудача') + body.format(login=login, password=password) + "</body>\n</html>"

print(get_response() + "\r\n\r\n" + main_page + "\r\n\r\n")