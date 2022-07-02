//
// Created by Grubbly Ernesto on 6/7/22.
//

#pragma once

# include "Utils.hpp"
# include "SimpSocket.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Config.hpp"

# define TIMEOUT 1

struct Server {
	struct	Client {
		int			socketFd;
		std::string	ip;
		int			acceptFd;
		sockaddr_in	address;

		Client(int socketFd, std::string const &ip, int acceptFd, sockaddr_in clAddr)
			: socketFd(socketFd), ip(ip), acceptFd(acceptFd), address(clAddr) {};

		int			getSocketFd()	const { return socketFd; };
		int			getAcceptFd()	const { return acceptFd; };
		sockaddr_in	getAddress ()	const { return address; };
		std::string	getIp ()		const { return ip; };
	};

	typedef std::vector<SimpSocket *>	Sockets;
	typedef std::map<int, Client*>		Clients;
	typedef std::map<int, Request *>	Requests;
	typedef std::map<int, Params>		Configs;
	typedef std::vector<struct kevent>	Kevent;

private:
	Configs			_configs;
	Sockets			_sockets;
	Clients			_clients;
	Requests		_requests;
	Kevent			_chList, _evList; // events we want to monitor, events that were triggered
	struct kevent	_test[1024];

	struct timespec	_timeout = { TIMEOUT, 0 };
	int				_kq;

	void		updateEvent(int socketFD, short filter, ushort flags, uint fflags, int data, void *udata, bool = false);
	void		acceptConnection(int);
	void		dropConnection(Client *, bool = true);
	int			kevent();
	void		accept(int);
//	void		handle();
	bool		receiver(Client *cl, long dataLen);
	bool		sender(Client *cl, size_t availBytes);
	bool		sender_test(Client *cl, size_t availBytes);

public:
	Server(char **av);
	Server(Server const &src);
	Server &operator = (Server const &src);
	virtual ~Server();

	Sockets		getServSocket() const { return this->_sockets; };

	void		runServer();

	struct	ConfigException: std::exception {
		const char	*what() const throw() {
			return "[Exception] Bad Config: check logs for more info";
		}
	};
	struct	SocketException: std::exception {
		const char	*what() const throw() {
			return "[System Exception] Socket: check logs for more info";
		}
	};
	struct	KqueueException: std::exception {
		const char	*what() const throw() {
			return "[System Exception] kqueue(): check logs for more info";
		}
	};
	struct	KeventException: std::exception {
		const char	*what() const throw() {
			return "[System Exception] kevent(): check logs for more info";
		}
	};
};
