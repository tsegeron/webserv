#!/usr/local/bin/python3
import html
import urllib.parse
from functions import *


def get_posts():
    dicti = dict()
    if os.environ.get('QUERY_STRING'):
        data = os.environ['QUERY_STRING'].split('&')
        for i in data:
            i = i.split('=')
            dicti[i[0]] = i[1]
    page = int(dicti.get('page')) if dicti.get('page') else 0
    posts_on_page = 1
    body = ""
    with sqlite3.connect("cgi-bin/web.db") as conn:
        cur = conn.cursor()
        cur.execute(f"""SELECT posts.img_path,  users.login, posts.description, posts.date_post, posts.id
                        FROM posts
                        JOIN users ON posts.author = users.id ORDER BY posts.date_post DESC
                        LIMIT {posts_on_page} OFFSET {page * posts_on_page};""")
        result = cur.fetchall()
        cur.execute(f"""SELECT *
                        FROM posts
                        JOIN users ON posts.author = users.id ORDER BY posts.date_post DESC
                        LIMIT {posts_on_page} OFFSET {(page + 1) * posts_on_page};""")
        result2 = cur.fetchall()
        for i in result:
            body += f"""
        <div class="card mb-3" style="max-width: 60%;">
            <div class="row g-0">
                <div class="col-md-4" style="max-width:30%;max-height:30%;">
                    <a href="{html.escape(i[0])}"><img src="{html.escape(i[0])}" style="max-width:100%; max-height: 100%;"></a>
                </div>
                <div class="col-md-8">
                    <div class="card-body">
                        <h5 class="card-title">{html.escape(i[1])}</h5>
                            <p class="card-text text-truncate"> {html.escape(i[2])}</p>
                        <button class="btn btn-outline-primary" type="button" data-bs-toggle="collapse" data-bs-target="#collapse_post_id{i[4]}" aria-expanded="false" aria-controls="collapse_post_id{i[4]}">
                            Полное описание
                        </button>
                        <div class="collapse" id="collapse_post_id{i[4]}">
                          <div class="card card-body" style="margin:20px">
                            <p class="card-text"> {html.escape(i[2])}</p>
                          </div>
                        </div>
                        <button class="btn btn-outline-primary" type="button" data-bs-toggle="offcanvas" data-bs-target="#post_id{i[4]}" aria-controls="post_id1">Показать комментарии</button>
                        <div class="offcanvas offcanvas-end" tabindex="-1" id="post_id{i[4]}" aria-labelledby="RightLabel{i[4]}">
                            <div class="offcanvas-header">
                                <h5 class="offcanvas-title" id="Rightlabel{i[4]}">Комментарии</h5>
                                <button type="button" class="btn-close" data-bs-dismiss="offcanvas" aria-label="Close"></button>
                            </div>"""
            cur.execute(f"""SELECT c.text, u.login, c.date FROM posts
                LEFT JOIN comments c on posts.id = c.post_ref
                LEFT JOIN users u on c.author = u.id
                WHERE posts.id={i[4]};""")
            res = cur.fetchall()
            body += "       <div class=\"offcanvas-body\">\n"
            if res[0][0] is None:
                body += """     <div class="card" style="margin-bottom: 20px">
                                    <div class="card-header">
                                        <strong> Нет комментариев</strong>
                                    </div>
                                </div>"""
            else:
                for j in res:
                    body += f"""
                                <div class="card" style="margin-bottom: 20px">
                                    <div class="card-header">
                                        <strong> Автор комментария : {j[1]}</strong>
                                    </div>
                                    <div class="card-body">
                                        <p>{j[0]}</p>
                                    </div>
                                    <div class="card-footer text-muted"> 
                                        Опубликован {j[2]} 
                                    </div>
                                </div>"""
            body += """     </div>
                        </div>"""
            body += f"""
                    <button type="button" class="btn btn-outline-primary " data-bs-toggle="modal" data-bs-target="#post_modal{i[4]}" {'disabled' if os.environ.get('LOGIN') is None else ''}>Добавить комментарий</button>
                    <div class="modal fade" id="post_modal{i[4]}" tabindex="-1" aria-labelledby="ModalLabel{i[4]}" aria-hidden="true">
                        <div class="modal-dialog">
                            <div class="modal-content">
                                <div class="modal-header">
                                    <h5 class="modal-title" id="ModalLabel{i[4]}">Добавить комметарий</h5>
                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                </div>
                                <div class="modal-body">
                                    <form method="post">
                                        <div class="mb-3">
                                            <label class="form-label">Комментарий</label>
                                            <textarea class="form-control" name="comment" rows="3"></textarea>
                                            <input type="hidden" name="id_post" value="{i[4]}">
                                        </div>
                                        <div class="modal-footer">
                                            <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Закрыть</button>
                                            <button type="submit" class="btn btn-primary">Отправить</button>
                                        </div>
                                    </form>
                                </div>
                            </div>
                        </div>
                    </div>"""
            body += f"""
                    </div>
                </div>
            </div>
            <div class="card-footer">
                <small class="text-muted">Дата публикации: {i[3]}</small>
            </div>
        </div>
    """
        body += f"""
        <div class="position-relative">
                <ul class="pagination">
                  <li class="page-item {'disabled' if page <= 0 else ''}"><a class="page-link" href="/home?page={page - 1}" >Предыдущая</a></li>
                  <li class="page-item {'disabled' if not result or not result2 else ''}"><a class="page-link" href="/home?page={page + 1}" >Следущая</a></li>
                </ul>   
        </div>
    """
    return body


auth = get_auth()

if os.environ['REQUEST_METHOD'] == 'GET':
    body = get_body('Дом')
    body += "<h3 class=\"display-5\" style=\"margin-left:10px;\">Все записи</h3>"
    body += get_posts()
    main_page = get_head_html('Домашняя страница') + body + "</body>\n</html>"
    print(get_response().format(status="200 OK",
                                server=os.environ['SERVER_NAME'],
                                cont_len=len(main_page.encode('UTF-8')),
                                cont_type=os.environ['CONTENT_TYPE']) +
          "\r\n\r\n" + main_page, end='')
elif not auth:
    with open("error_pages/403.html", 'r') as file:
        main_page = file.read()
        print(get_response().format(status="403 Forbidden",
                                    server=os.environ['SERVER_NAME'],
                                    cont_len=len(main_page.encode('utf-8')),
                                    cont_type=os.environ['CONTENT_TYPE']) + f"\nSet-Cookie:user={os.environ.get('LOGIN')};Max-age=0" + "\r\n\r\n" + main_page, end='')
else:
    body = get_body('Дом')
    data = os.environ['QUERY_STRING'].split('&')
    dicti = dict()
    for i in data:
        i = i.split('=')
        dicti[i[0]] = i[1]
    text = urllib.parse.unquote_plus(dicti.get('comment')) if dicti.get('comment') else None
    id_post = int(urllib.parse.unquote_plus(dicti.get('id_post'))) if dicti.get('id_post') else None
    with sqlite3.connect("cgi-bin/web.db") as conn:
        cur = conn.cursor()
        cur.execute(f"SELECT * FROM posts WHERE posts.id = {id_post}")
        res = cur.fetchone()
        if not all([text, id_post]) or res is None:
            body += """<div class="alert alert-danger alert-dismissible fade show" role="alert">
      <strong>Некорректные данные формы </strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>""" + "<h3 class=\"display-5\" style=\"margin-left:10px;\">Все записи</h3>" + get_posts()
            main_page = get_head_html('Ошибка') + body + "</body>\n</html>"
        else:

            cur.execute(f"SELECT users.id FROM users WHERE login = '{os.environ.get('LOGIN')}';")
            result = cur.fetchone()
            author = result[0]
            cur.execute(f"""INSERT INTO comments(author, text, post_ref)
                    VALUES
                    (?, ?, ?);""", (author, text, id_post))
            conn.commit()
            body += """<div class="alert alert-success alert-dismissible fade show" role="alert">
      <strong>Комментарий успешно добавлен </strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>""" + "<h3 class=\"display-5\" style=\"margin-left:10px;\">Все записи</h3>\n" + get_posts()
            main_page = get_head_html('Дом') + body + "</body>\n</html>"
    print(get_response().format(status="200 OK",
                                server=os.environ['SERVER_NAME'],
                                cont_len=len(main_page.encode('utf-8')),
                                cont_type=os.environ['CONTENT_TYPE']) +
          "\r\n\r\n" + main_page, end='')


