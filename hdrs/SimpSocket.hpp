//
// Created by Grubbly Ernesto on 5/7/22.
//

#ifndef SIMPSOCKET_HPP
# define SIMPSOCKET_HPP

# include "Utils.hpp"

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

	bool	setSocketAsNonblock();
	bool	allowMultipleConnectionsOnSocket();
	bool	bindSocketToLocalSockaddr();
	bool	initiateConnectionOnSocket(); // for client
	bool	listenForConnectionsOnSocket();

	std::string					getIP() const { return host + ":" + std::to_string(port); };
	int							getServerFd() const { return serverFd; };
	struct sockaddr_in			getAddress() const { return address; };
	int							getAddressLen() const { return addrLen; };

	struct	InetAddrException: std::exception {
		const char	*what() const throw() {
			return "Socket: bad host format";
		}
	};
	struct	SocketException: std::exception {
		const char	*what() const throw() {
			return "Socket: error in 'socketFd' function";
		}
	};
};

#endif //SIMPSOCKET_HPP
