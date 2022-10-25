#!/usr/bin/env python3
import subprocess

from functions import *

if not get_auth():
    subprocess.run(['python3', 'cgi-bin/home.py'])
else:
    print(f"""HTTP/1.1 301 Moved Permanently\r
    Server: {os.environ['SERVER_NAME']}\r
    Location: http://{os.environ['REMOTE_HOST']}:{os.environ['REMOTE_PORT']}/profile \r
    Content-Length: 0\r
    Content-Type: text/html\r\n\r\n""")
