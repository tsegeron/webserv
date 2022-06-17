# #!/usr/bin/env python3
# import os
# import cgi
#
#
# form = cgi.FieldStorage()
#
# # Get filename here.
# fileitem = form['file']
#
# if fileitem.filename:
#     fn = os.path.basename(fileitem.filename)
#     open('/tmp/' + fn, 'wb').write(fileitem.file.read())
#
#     message = 'The file "' + fn + '" was uploaded successfully'
#
# else:
#     message = 'No file was uploaded'
#
# print("""HTTP/1.1 200 OK\r
# Server: {server}\r
# Content-Type: {cont_type}\r
# Content-Length: {cont_len}\r\n\r\n""".format(server=os.environ['SERVER_NAME'],
#                                              cont_len=len(message),
#                                              cont_type=os.environ['CONTENT_TYPE']) + message + "\r\n\r\n")