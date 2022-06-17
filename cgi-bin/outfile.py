#!/usr/bin/env python3
import html
from functions import *

with open("pages/style.css", 'r') as file:
    s = file.read()

print(get_response().format(server=os.environ['SERVER_NAME'],
                            cont_len=len(s.encode('utf-8')),
                            cont_type=os.environ['CONTENT_TYPE']) + "\r\n\r\n" + s + "\r\n\r\n")