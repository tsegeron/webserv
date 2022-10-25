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

	memset(&address, 0, sizeof address);

	address.sin_family		= domain;
	address.sin_addr.s_addr	= inet_addr(host.c_str());
	address.sin_port		= htons(port);
	addrLen					= sizeof(address);

	serverFd				= socket(domain, type, protocol);

	if (serverFd == -1)
	{
		utils::logging("srcs/SimpSocket.cpp:26: error: socket()", utils::error);
		throw SocketException();
	}
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

bool	SimpSocket::setSocketAsNonblock()
{
	if (::fcntl(serverFd, F_SETFL, O_NONBLOCK) == -1)
	{
		utils::logging("srcs/SimpSocket.cpp:66: error: fcntl()", utils::error);
		return false;
	}
	return true;
}

bool	SimpSocket::allowMultipleConnectionsOnSocket()
{
	int	opt = 1;

	if (::setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
	{
		utils::logging("srcs/SimpSocket.cpp:78: error: setsockopt()", utils::error);
		return false;
	}
	return true;
}

bool	SimpSocket::bindSocketToLocalSockaddr()
{
	if (::bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		utils::logging("srcs/SimpSocket.cpp:88: error: bind()", utils::error);
		return false;
	}
	return true;
}

bool	SimpSocket::initiateConnectionOnSocket()
{
	address.sin_addr.s_addr = ::inet_addr(host.c_str());
	if (address.sin_addr.s_addr < 0 || ::connect(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		utils::logging("srcs/SimpSocket.cpp:98-99: error: inet_addr() or connect()", utils::error);
		return false;
	}
	return true;
}

bool	SimpSocket::listenForConnectionsOnSocket()
{
	if (::listen(serverFd, SOMAXCONN) < 0)
	{
		utils::logging("srcs/SimpSocket.cpp:109: error: listen()", utils::error);
		return false;
	}
	return true;
}
