#!/usr/bin/env python3
import html
from functions import *


def get_my_posts():
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
                        JOIN users ON posts.author = users.id 
                        WHERE users.login = '{os.environ['LOGIN']}' 
                        ORDER BY posts.date_post DESC
                        LIMIT {posts_on_page} OFFSET {page * posts_on_page};""")
        result = cur.fetchall()
        cur.execute(f"""SELECT *
                        FROM posts
                        JOIN users ON posts.author = users.id
                        WHERE users.login = '{os.environ['LOGIN']}'  
                        ORDER BY posts.date_post DESC
                        LIMIT {posts_on_page} OFFSET {(page + 1) * posts_on_page};""")
        result2 = cur.fetchall()
        for i in result:
            body += f"""
    <div class="card mb-3" style="max-width: 60%;">
        <div class="row g-0">
            <div class="col-md-4" style="max-width:30%;max-height:30%;">
                <a href="{html.escape(i[0])}">
                    <img src="{html.escape(i[0])}" style="max-width: 100%; max-height: 100%;">
                </a>
            </div>
            <div class="col-md-8">
                <div class="card-body">
                    <h5 class="card-title">Автор поста: {html.escape(i[1])}</h5>
                    <p class="card-text" > Описание поста: {html.escape(i[2])}</p>
                    
                    <button class="btn btn-outline-primary" style="margin:5px; width:300px;" type="button" data-bs-toggle="offcanvas" data-bs-target="#post_id{i[4]}" aria-controls="post_id1">Показать комментарии</button>
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
                                        <strong> Автор комментария : {html.escape(j[1])}</strong>
                                    </div>
                                    <div class="card-body">
                                        <p>{html.escape(j[0])}</p>
                                    </div>
                                    <div class="card-footer text-muted"> 
                                        Опубликован {j[2]} 
                                    </div>
                                </div>"""
            body += f"""     </div>
                        </div>
                        <form method="post" action="/rm_post">
                            <input type="hidden" name="id_post" value="{i[4]}">
                            <button type="submit" class="btn btn-outline-danger" style="margin:5px; width:300px;">Удалить публикацию</button>
                        </form>
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
              <li class="page-item {'disabled' if page <= 0 else ''}"><a class="page-link" href="?page={page - 1}" >Предыдущая</a></li>
              <li class="page-item {'disabled' if not result or not result2 else ''}"><a class="page-link" href="?page={page + 1}" >Следущая</a></li>
            </ul>   
    </div>
"""
    return body


if not get_auth():
    with open("error_pages/403.html", 'r') as file:
        main_page = file.read()
        print(get_response().format(status="403 Forbidden",
                                    server=os.environ['SERVER_NAME'],
                                    cont_len=len(main_page.encode('utf-8')),
                                    cont_type=os.environ['CONTENT_TYPE']) + "\r\n\r\n" + main_page, end='')
else:
    body = get_body('Мой профиль')
    body += "<h3 class=\"display-5\" style=\"margin-left:10px;\">Мои записи</h3>"
    conn = sqlite3.connect("cgi-bin/web.db")
    cur = conn.cursor()
    cur.execute(f"""SELECT img_path, description, date_post, u.login
        FROM posts
        JOIN users u ON posts.author = u.id
        where u.login = '{os.environ.get('LOGIN')}';""")
    result = cur.fetchall()
    if not result:
        body += "<h3 style=\"margin-left:10px;\">У вас нет опубликованных записей</h3>\n"
    else:
        body += get_my_posts()
    main_page = get_head_html('Мой профиль') + body + "</body>\n</html>"

    print(get_response().format(status="200 OK",
                                server=os.environ['SERVER_NAME'],
                                cont_len=len(main_page.encode('utf-8')),
                                cont_type=os.environ['CONTENT_TYPE']) +
          "\r\n\r\n" + main_page, end='')
