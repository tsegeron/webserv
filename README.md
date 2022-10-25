# Webserv
*Writing an HTTP server from scratch*

## Description
- A HTTP server, fully written in `C++98`
- CGI-es for the server are written on `Python`
- Frontend is based on `bootstrap5 styles`
- `SQLite` is used to save *logins, passwords, comments*
- ***Cookies*** and ***session management*** are supported
- HTTP response *status codes* are accurate
- Default *error pages* are provided
- Clients are able to *upload files*
- `Chunked` transfer encoding is supported
- Implemented methods: `HEAD`, `GET`, `POST`, `PUT`, `DELETE`
- Polling is implemented with `kqueue` (`kevent`)

## How to use
1. Clone the repo
```
git clone git@github.com:tsegeron/webserv.git && cd webserv
```
2. Build a project
```
make
```
3. Launch the server `./webserv [config/deafult.config]`, you can pass your own config.
If none is provided it will use the default one. `*.config` extension is essential.
```
./webserv config/deafult.config
```
4. Click the URL the server is started on
5. If `500. Server error is occured` try configuring python interpreter to the 3.*v
and try different URLs from config file. 