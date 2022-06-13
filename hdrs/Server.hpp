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
# define BACKLOG 100

struct Server: IServer {
	typedef typename std::vector<SimpSocket *>	Sockets;
	typedef typename std::map<int, Request *>	Requests;
	typedef typename std::map<int, Params>		Listen;
private:
	Sockets			_sockets;
//	Config			*_config;
	Requests		_requests;
//	Urls			*_urls;
	fd_set			_currentSockets, _readSockets, _writeSockets;
	Listen			_fds;
	struct timeval	_timeout;

	void		accepter(struct sockaddr_in &, int &) final;
	void		handler(int) final;
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

	Sockets		getServSocket() const { return this->_sockets; };

	void		runServer() final;

	struct	ConfigException: std::exception {
		const char	*what() const throw() {
			return "[Exception] Bad Config: check logs file for more info";
		}
	};
	struct	SocketException: std::exception {
		const char	*what() const throw() {
			return "[System Exception] Socket: check logs file for more info";
		}
	};
};


#endif //ZERO_WEBSERV_SERVER_HPP
