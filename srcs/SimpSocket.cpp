//
// Created by Grubbly Ernesto on 5/7/22.
//

#include "../hdrs/Utils.hpp"
#include "../hdrs/SimpSocket.hpp"

// Default constructor
SimpSocket::SimpSocket(std::string const &host, int port)
	: host(host), port(port)
{
	domain					= AF_INET;
	type					= SOCK_STREAM;
	protocol				= 0;
	interface				= INADDR_ANY;
	address.sin_family		= domain;
	address.sin_addr.s_addr	= htonl(interface);
	address.sin_port		= htons(port);
	serverFd				= socket(domain, type, protocol);

	addrLen					= sizeof(address);

	memset(address.sin_zero, 0, sizeof address.sin_zero);
}

SimpSocket::SimpSocket(const SimpSocket &other)
{
	*this = other;
}

SimpSocket & SimpSocket::operator=(SimpSocket const &other)
{
	if (this != &other)
	{
		domain		= other.domain;
		type		= other.type;
		protocol	= other.protocol;
		host		= other.host;
		port		= other.port;
		interface	= other.interface;
		serverFd	= other.serverFd;
		address		= other.address;
		addrLen		= other.addrLen;
	}
	return *this;
}

SimpSocket::~SimpSocket()
{
	close(serverFd);
}

// returns true if an endpoint for communication was successfully created
bool	SimpSocket::is_socket_valid()
{
	if (serverFd < 0)
	{
		utils::logging("socket: " + std::string(strerror(errno)), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::allowMultipleConnectionsOnSocket()
{
	int	opt = true;

	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&opt), sizeof(opt)) < 0)
	{
		utils::logging("setsockopt: " + std::string(strerror(errno)), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::bindSocketToLocalSockaddr()
{
	if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		utils::logging("bind: " + std::string(strerror(errno)), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::initiateConnectionOnSocket()
{
	address.sin_addr.s_addr = inet_addr(host.c_str());
	if (address.sin_addr.s_addr < 0 || connect(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		utils::logging("inet_addr: " + std::string(strerror(errno)), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::listenForConnectionsOnSocket(int bcklg)
{
	if (listen(serverFd, bcklg) < 0)
	{
		utils::logging("listen: " + std::string(strerror(errno)), 2);
		return false;
	}
	return true;
}
