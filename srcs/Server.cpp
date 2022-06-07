#include "../hdrs/Server.hpp"

Server::Server(int domain, int type, int protocol, int port, u_long interface, int backlog)
{
	_servSocket = new SimpSocket(domain, type, protocol, port, interface);

	_servSocket->createSocket();
	_servSocket->allowMultipleConnectionsOnSocket();
	_servSocket->bindSocketToLocalSockaddr();
	_servSocket->listenForConnectionsOnSocket(backlog);

	_timeout.tv_sec = TIMEOUT;
	_timeout.tv_usec = 0;
	utils::openLogFile();
	FD_ZERO(&_currentSockets);
	FD_ZERO(&_readSockets);
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
	this->_readSockets = src._readSockets;

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

void	Server::accepter(struct sockaddr_in &address, int &addrLen)
{
	std::string			dots[4] = {"", ".", "..", "..."};
	struct timeval		timeout;
	int					socket;
	int					selectStat = 0;
	int					n = -1;

	while (selectStat == 0)
	{
		timeout = _timeout;
		std::cout << YELLOW << "\33[2\rKWaiting for connection" << dots[++n] << RESET << std::flush;
		if (n == 3)
			n = -1;
		_readSockets = _currentSockets;
		FD_ZERO(&_writeSockets);
		selectStat = select(FD_SETSIZE, &_readSockets, &_writeSockets, nullptr, &timeout); // max num of sockets' fds
	}
	std::cout << std::endl;
	if (selectStat == -1)
		utils::logging(strerror(errno));
	else
	{
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &_readSockets))
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
	}
}

void	Server::handler(long clientSocket)
{
	char	buffer[10000];

	if (recv(int(clientSocket), buffer, 10000, 0) < 0)
		utils::logging("Server: recv failed", 2);
	std::cout << MAGENTA << "---------Reading request---------" << RESET << std::endl;
	std::cout << buffer << std::endl;
	std::cout << MAGENTA << "---------------------------------" << RESET << std::endl;
}

void	Server::responder()
{

}
