//
// Created by Grubbly Ernesto on 5/7/22.
//

#include "../hdrs/Utils.hpp"
#include "../hdrs/SimpSocket.hpp"

// Default constructor
SimpSocket::SimpSocket(int domain, int type, int protocol,
							int port, u_long interface)
		: domain(domain), type(type), protocol(protocol), port(port), interface(interface)
{
	// defining sockaddr_in structure
	address.sin_family		= domain;
	address.sin_addr.s_addr	= htonl(interface);
	address.sin_port		= htons(port);
	insertSocketParams();

	addrLen					= sizeof(address);

	memset(address.sin_zero, 0, sizeof address.sin_zero);
}

void	SimpSocket::insertSocketParams()
{
	socketParameters.insert(std::pair<std::string, int>("domain", domain));
	socketParameters.insert(std::pair<std::string, int>("type", type));
	socketParameters.insert(std::pair<std::string, int>("protocol", protocol));
	socketParameters.insert(std::pair<std::string, int>("port", port));
	socketParameters.insert(std::pair<std::string, int>("interface", int(interface)));
}

SimpSocket::SimpSocket(const SimpSocket &other)
{
	*this = other;
}

SimpSocket & SimpSocket::operator=(SimpSocket const &other)
{
	if (this != &other)
	{
		this->serverFd = other.serverFd;
		this->port = other.port;
		this->interface = other.interface;
		this->socketParameters = other.socketParameters;
	}
	return *this;
}

SimpSocket::~SimpSocket()
{
	close(serverFd);
}

// returns true if an endpoint for communication was successfully created
bool	SimpSocket::createSocket()
{
	serverFd = socket(domain, type, protocol);
	if (serverFd < 0)
	{
		utils::logging(strerror(errno), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::allowMultipleConnectionsOnSocket()
{
	int	opt = true;

	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(&opt), sizeof(opt)) < 0)
	{
		utils::logging(strerror(errno), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::bindSocketToLocalSockaddr()
{
	if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		utils::logging(strerror(errno), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::initiateConnectionOnSocket()
{
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (address.sin_addr.s_addr < 0 || connect(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		utils::logging(strerror(errno), 2);
		return false;
	}
	return true;
}

bool	SimpSocket::listenForConnectionsOnSocket(int bcklg)
{
	setBacklog(bcklg);

	if (listen(serverFd, bcklg) < 0)
	{
		utils::logging(strerror(errno), 2);
		return false;
	}
	return true;
}
