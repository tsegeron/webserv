#include "../hdrs/Server.hpp"

Server::Server(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	_servSocket = new SimpSocket(domain, type, protocol, port, interface);

	_servSocket->createSocket();
	_servSocket->allowMultipleConnectionsOnSocket();
	_servSocket->bindSocketToLocalSockaddr();
	_servSocket->listenForConnectionsOnSocket(backlog);

	FD_ZERO(&_currentSockets);
	FD_ZERO(&_readySockets);
	FD_SET(_servSocket->getServerFd(), &_currentSockets);
}

Server::Server(Server const &src)
{
	*this = src;
}

Server &Server::operator = (const Server &src)
{
	this->_servSocket = src._servSocket;
	this->_currentSockets = src._currentSockets;
	this->_readySockets = src._readySockets;

	return *this;
}

Server::~Server()
{
	delete _servSocket;
	_servSocket = nullptr;
	utils::closeLogFile();
}

void	Server::runServer()
{
	struct sockaddr_in	address	= _servSocket->getAddress();
	int					addrLen	= _servSocket->getAddressLen();

	while (true)
		accepter(address, addrLen);
}

bool	Server::accepter(struct sockaddr_in address, int addrLen)
{
	int					socket;

	_readySockets = _currentSockets;
	if (select(FD_SETSIZE, &_readySockets, nullptr, nullptr, nullptr) == -1)
		utils::logging(strerror(errno));

	for (int i = 0; i < FD_SETSIZE; i++)
	{
		if (FD_ISSET(i, &_readySockets))
		{
			if (i == _servSocket->getServerFd())
			{
				socket = accept(_servSocket->getServerFd(),
								(struct sockaddr *)&address,
								(socklen_t *)&addrLen);
				FD_SET(socket, &_currentSockets);
			}
			else
			{
				handler(i);
				FD_CLR(i, &_currentSockets);
			}
		}
	}

	return true;
}

void	Server::handler(long clientSocket)
{
	char	buffer[10000];

	if (recv(int(clientSocket), buffer, 10000, 0) < 0)
		utils::logging("Server: recv failed", 2);
	std::cout << buffer << std::endl;
}

void	Server::responder()
{

}
