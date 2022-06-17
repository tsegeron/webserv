import os


def get_nav():
    nav = str()
    if not os.environ['LOGIN']:
        with open('pages/nav.html') as html:
            for row in html:
                nav += row
    else:
        with open('pages/nav_log.html') as html:
            for row in html:
                nav += row
    return nav


def get_head_html(title):
    head = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{title}</title>
</head>
"""
    return head


def get_logins():
    with open("users.txt", 'r') as file:
        lst = file.read().split('\n')
        return lst


def get_passwords():
    with open("passwords.txt", 'r') as file:
        lst = file.read().split('\n')
        return lst


def get_response():
    tmp = """HTTP/1.1 200 OK\r
Server: {server}\r
Content-Type: {cont_type}\r
Content-Length: {cont_len}"""
    return tmp


def get_body():
    tmp = """<body>
    <p>Работаем на сервере: {serv}</p>
{nav}
    """.format(nav=get_nav().format(login=os.environ['LOGIN']), serv=os.environ['SERVER_NAME'])
    return tmp