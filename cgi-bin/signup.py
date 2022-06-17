#!/usr/bin/env python3

from functions import *
import cgi

body = get_body()
if os.environ['REQUEST_METHOD'] == 'GET':
    body += """<form action="/form_signup" method="post">
        <p><input type="text" name="login" required minlength="8" maxlength="16" placeholder="Login" autocomplete="false"></p>
        <p><input type="text" name="password1" required minlength="8" maxlength="16" placeholder="Password""></p>
        <p><input type="text" name="password2" required minlength="8" maxlength="16" placeholder="Confirm password""></p>
        <input type="submit" value="Зарегистрироваться">
    </form>"""
    main_page = get_head_html('Регистрация') + body + "</body>\n</html>"

# else:
#     form = cgi.FieldStorage()
#
#     login = form.getvalue('login')
#     password1 = form.getvalue('password1')
#     password2 = form.getvalue('password2')
#
#     if login not in get_logins() or password1 != password2:
#         body += """<p>Ошибка</p><form action="/sign_up" method="post">
#         <p><input type="text" name="login" required minlength="8" maxlength="16" placeholder="Login" autocomplete="false"></p>
#         <p><input type="password" name="password1" required minlength="8" maxlength="16" placeholder="Password""></p>
#         <p><input type="password" name="password2" required minlength="8" maxlength="16" placeholder="Confirm password""></p>
#         <input type="submit" value="Зарегистрироваться">
#     </form>"""
#         main_page = get_head_html('Пользователь не создан') + body + "</body>\n</html>"
#     else:
#         with open("users.txt", "a") as file:
#             file.write(login + '\n')
#         with open("passwords.txt", "a") as file:
#             file.write(password1 + '\n')
#         body = """<h2>Привет {login}, {password}</h2>""".format(login=login,
#                                                                 password=password1)
#         main_page = get_head_html('Пользователь успешно создан') + body + "</body>\n</html>"

print(get_response().format(server=os.environ['SERVER_NAME'],
                            cont_type=os.environ['CONTENT_TYPE'],
                            cont_len=len(main_page.encode('utf-8'))) +
      "\r\n\r\n" + main_page + "\r\n\r\n")