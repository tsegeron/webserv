//
// Created by Grubbly Ernesto on 6/7/22.
//

#ifndef ZERO_WEBSERV_SERVER_HPP
# define ZERO_WEBSERV_SERVER_HPP

# include "Utils.hpp"
# include "IServer.hpp"
# include "Request.hpp"
# include "Config.hpp"
# include "Urls.hpp"

# define TIMEOUT 1
# define BACKLOG 1

struct Server: IServer {
private:
	SimpSocket		_servSocket;
	Config			_config;
	Request			_request;
//	Urls			*_urls;
	fd_set			_currentSockets, _readSockets, _writeSockets;
	struct timeval	_timeout;

	void		accepter(struct sockaddr_in &, int &) final;
	void		handler(long) final;
	void		responder() final;

public:
//	Server(int domain = AF_INET,
//		   int type = SOCK_STREAM,
//		   int protocol = 0,
//		   int port = 8080,
//		   u_long interface = INADDR_ANY,
//		   int backlog = 1000);
	Server(char **av);
	Server(Server const &src);
	Server &operator = (Server const &src);
	virtual ~Server();

	SimpSocket	getServSocket() const final { return this->_servSocket; };

	void		runServer() final;

//	struct	ConfigException: std::exception {
//		const char	*what() const throw() {
//			return "[Exception] Config: check logs file for more info";
//		}
//	};
};


#endif //ZERO_WEBSERV_SERVER_HPP
