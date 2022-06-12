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
	std::string					host;
	int							port;
	u_long						interface;

	int							serverFd;
	struct sockaddr_in			address;
	int							addrLen;


public:
	SimpSocket(std::string const &, int);
	SimpSocket(SimpSocket const &);
	SimpSocket & operator = (SimpSocket const &);
	virtual ~SimpSocket();

	bool	is_socket_valid();
	bool	allowMultipleConnectionsOnSocket();
	bool	bindSocketToLocalSockaddr();
	bool	initiateConnectionOnSocket();
	bool	listenForConnectionsOnSocket(int backlog);

	int							getServerFd() const { return serverFd; };
	struct sockaddr_in			getAddress() const { return address; };
	int							getAddressLen() const { return addrLen; };

};

#endif //SIMPSOCKET_HPP
