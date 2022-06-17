#!/usr/bin/env python3

import os
import sys
print("""HTTP/1.1 200 OK\r
    Server: {server}\r
    Content-Type: {cont_type}\r
    Content-Length: {cont_len}\r\n\r\n""".format(server=os.environ['SERVER_NAME'],
                                                 cont_len=os.environ['CONTENT_LENGTH'],
                                                 cont_type="image/png"))
# from functions import *
#
# with open("images/1.png", 'rb') as f:
#     data = f.read()
#     print(get_response().format(server=os.environ['SERVER_NAME'],
#                                 cont_type=os.environ['CONTENT_TYPE'],
#                                 cont_len=len(data)))
#     print(data)
