#!/usr/local/bin/python3
import html
from functions import *
if not get_auth():
    with open("error_pages/403.html", 'r') as file:
        main_page = file.read()
        print(get_response().format(status="403 Forbidden",
                                    server=os.environ['SERVER_NAME'],
                                    cont_len=len(main_page.encode('utf-8')),
                                    cont_type=os.environ['CONTENT_TYPE']) + "\r\n\r\n" + main_page, end='')
else:
    body = get_body('Автоиндекс')

    path = os.environ.get('FILENAME')
    if os.path.isdir(path):
        body += "<h3 class=\"display-5\" style=\"margin-left:10px;\">Файлы и папки в директории</h3>"
        body += "<div style=\"margin:20px;\">"
        for i in os.listdir(path):
            body += f"\t<a href=\"/{path}/{i}\">{i}</a><br/>\n"
        main_page = get_head_html('Автоиндекс') + body + "</div>\n</body>\n</html>"
        print(get_response().format(status="200 OK",
                                    server=os.environ['SERVER_NAME'],
                                    cont_type="*/*",
                                    cont_len=len(main_page.encode('utf-8'))) +
              "\r\n\r\n" + main_page)
    elif os.path.isfile(path):
        body += "<h3 class=\"display-5\" style=\"margin-left:10px;\">Содержимое файла</h3>"
        body += "<div style=\"margin:20px;\">"
        try:
            with open(path, 'r') as f:
                for i in f:
                    body += html.escape(i) + "</br>\n"
                main_page = get_head_html('Содержимое файла') + body + "</body>\n</html>"
                print(get_response().format(status="200 OK",
                                            server=os.environ['SERVER_NAME'],
                                            cont_type=os.environ['CONTENT_TYPE'],
                                            cont_len=len(main_page.encode())) + "\r\n\r\n" + main_page)
        except Exception:
            # with open(path, 'rb') as f:
            #     main_page = f.read()
            #     body = get_head_html(path) + body
            #     head = get_response().format(status="200 OK",
            #                                  server=os.environ['SERVER_NAME'],
            #                                  cont_type=os.environ['CONTENT_TYPE'],
            #                                  cont_len=len(body.encode()) + len(main_page)) + "\r\n\r\n"
            #     print(head + body, end='')
            #     for i in main_page:
            #         print(i, end='')
            #     print(f"\n<p>{path}</p>\n</body>\n</html>", end='')
            with open("error_pages/415.html", 'r') as file:
                main_page = file.read()
            print(get_response().format(status="415 Unsupported Media Type",
                                        server=os.environ['SERVER_NAME'],
                                        cont_len=len(main_page.encode('utf-8')),
                                        cont_type=os.environ['CONTENT_TYPE']) + "\r\n\r\n" + main_page, end='')
    else:
        with open("error_pages/404.html", 'r') as file:
            main_page = file.read()
            print(get_response().format(status="404 Not Found",
                                        server=os.environ['SERVER_NAME'],
                                        cont_len=len(main_page.encode('utf-8')),
                                        cont_type=os.environ['CONTENT_TYPE']) + "\r\n\r\n" + main_page, end='')
