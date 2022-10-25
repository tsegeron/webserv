#!/usr/bin/env python3
import html
from functions import *
import urllib.parse




if not get_auth():
    with open("error_pages/403.html", 'r') as file:
        main_page = file.read()
        print(get_response().format(status="403 Forbidden",
                                    server=os.environ['SERVER_NAME'],
                                    cont_len=len(main_page.encode('utf-8')),
                                    cont_type=os.environ['CONTENT_TYPE']) +
              "\r\n\r\n" + main_page, end='')
else:
    if os.environ['REQUEST_METHOD'] == 'POST':
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
      <strong>Пост успешно добавлен </strong>
      <button type="button" class="btn-close" data-bs-dismiss="alert" aria-label="Close"></button>
    </div>"""
            main_page = get_head_html('Пост опубликован') + body + "</body>\n</html>"
    else:
        body = get_body('Новая запись')
        form = """
        <h3 class=\"display-5\" style=\"margin-left:10px;\">Заполните форму</h3>
        <div class="accordion accordion-flush" id="accordionFlushExample">
            <div class="accordion-item">
                <h2 class="accordion-header" id="flush-headingOne">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#flush-collapseOne" aria-expanded="false" aria-controls="flush-collapseOne">
                        Загрузить файл   
                    </button>
                </h2>
                <div id="flush-collapseOne" class="accordion-collapse collapse" aria-labelledby="flush-headingOne" data-bs-parent="#accordionFlushExample">
                    <div class="accordion-body">
                            <form method="post" action="/add_image" enctype="multipart/form-data">
                                <div class="mb-3">
                                  <label class="form-label" >Выберите изображение</label>
                                  <input class="form-control" type="file" name="file" required multiple accept="image/*">
                                </div>
                                <div class="form-floating">
                                    <textarea class="form-control" placeholder="Добавить описание" style="height: 100px" name="description" required></textarea>
                                    <label>Описание</label>
                                </div>
                                <button type="submit" class="btn btn-primary" style="margin-top:10px;">Выложить</button>
                            </form>
                    </div>
                </div>
            </div>
            
            <div class="accordion-item">
                <h2 class="accordion-header" id="flush-headingOne">
                    <button class="accordion-button collapsed" type="button" data-bs-toggle="collapse" data-bs-target="#flush-collapseTwo" aria-expanded="false" aria-controls="flush-collapseTwo">
                        Загрузить URL изображения  
                    </button>
                </h2>
                <div id="flush-collapseTwo" class="accordion-collapse collapse" aria-labelledby="flush-headingOne" data-bs-parent="#accordionFlushExample">
                    <div class="accordion-body">
                        <form method="post" style="max-width:60%; margin:20px;">
                            <div class="form-floating" style="margin-bottom:20px;">
                                <input type="text" class="form-control" placeholder="Путь" name="img_path">
                                <label>URL изображения</label>
                            </div>
                            <div class="form-floating">
                                <textarea class="form-control" placeholder="Добавить описание" style="height: 100px" name="description"></textarea>
                                <label>Описание</label>
                            </div>
                            <button type="submit" class="btn btn-primary" style="margin-top:10px;">Выложить</button>
                        </form>
                    </div>
                </div>
            </div>
        </div>
        """
        body += form
        main_page = get_head_html('Публикация поста') + body + "</body>\n</html>"

    print(get_response().format(status="200 OK",
                                server=os.environ['SERVER_NAME'],
                                cont_len=len(main_page.encode('utf-8')),
                                cont_type=os.environ['CONTENT_TYPE']) +
          "\r\n\r\n" + main_page, end='')


