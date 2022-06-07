//
// Created by Grubbly Ernesto on 5/7/22.
//

#ifndef SIMPSOCKET_HPP
# define SIMPSOCKET_HPP

# include "Utils.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <exception>
# include <cstring>
# include <unistd.h>
# include <map>


class SimpSocket {
private:
	int							domain;
	int							type;
	int							protocol;
	int							port;
	u_long						interface;
	int							backlog;
	std::map<std::string, int>	socketParameters;

	int							serverFd;
	struct sockaddr_in			address;
	int							addrLen;

	void	insertSocketParams();

public:
	SimpSocket(int domain, int type, int protocol,
			   int port, u_long interface);
	SimpSocket(SimpSocket const &);
	SimpSocket & operator = (SimpSocket const &);
	virtual ~SimpSocket();

	bool	createSocket();
	bool	allowMultipleConnectionsOnSocket();
	bool	bindSocketToLocalSockaddr();
	bool	initiateConnectionOnSocket();
	bool	listenForConnectionsOnSocket(int backlog);

	std::map<std::string, int>	getSocketParameters() const { return socketParameters; };
	int							getServerFd() const { return serverFd; };
	struct sockaddr_in			getAddress() const { return address; };
	int							getAddressLen() const { return addrLen; };

	void						setBacklog(int n) { backlog = n; };

};

#endif //SIMPSOCKET_HPP
