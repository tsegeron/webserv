#!/usr/bin/env python3
from functions import *

with open("pages/style.css", 'r') as f:
    data = f.read()
    print(get_response().format(server=os.environ['SERVER_NAME'],
                                cont_len=len(data),
                                cont_type=os.environ['CONTENT_TYPE']))
    print(data)
