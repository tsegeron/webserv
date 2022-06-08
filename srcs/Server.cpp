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
	this->_servSocket		= src._servSocket;
	this->_currentSockets	= src._currentSockets;
	this->_readSockets		= src._readSockets;

	return *this;
}

Server::~Server()
{
	delete	_servSocket;
	delete	_request;
	delete	_urls;
	_servSocket	= nullptr;
	_request	= nullptr;
	_urls		= nullptr;
//	utils::closeLogFile();
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
	struct timeval		timeout{};
	int					socket;
	int					selectStat = 0;
	int					n = -1;

	while (selectStat == 0)
	{
		timeout = _timeout;
		std::cout << YELLOW << "\33[2K\rWaiting for connection" << dots[++n] << RESET << std::flush;
		if (n == 3)
			n = -1;
		_readSockets = _currentSockets;
		FD_ZERO(&_writeSockets);
		selectStat = select(FD_SETSIZE, &_readSockets, &_writeSockets, nullptr, &timeout); // max num of sockets' fds
	}
	std::cout << "\33[2K\r";
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

void	print_rawRequest(std::string const &request)
{
	std::cout << MAGENTA << "---------Reading request---------" << RESET << std::endl;
	std::cout << request << std::endl;
	std::cout << request.length() << std::endl;
	std::cout << MAGENTA << "---------------------------------" << RESET << std::endl;
}

void	print_fullRequest(std::map<std::string, std::string> request)
{
	std::cout << MAGENTA << "---------Reading request---------" << RESET << std::endl;
	for (const auto &elem : request)
		std::cout << elem.first << " : " << elem.second << std::endl;
	std::cout << MAGENTA << "---------------------------------" << RESET << std::endl;
}

void	print_shortRequest(Request const *request)
{
	std::cout << MAGENTA << "---------Reading request---------" << RESET << std::endl;
//	for (const auto &elem : request)
//		std::cout << elem.first << " : " << elem.second << std::endl;
	std::cout	<< request->getMethod() << " "
				<< request->getPath() << " "
				<< request->getAccept() << std::endl
				<< "Body: " << request->getBody() << std::endl;
//				<< request->getCookie() << std::endl;
	std::cout << MAGENTA << "---------------------------------" << RESET << std::endl;
}

void	Server::handler(long clientSocket)
{
	char	buffer[10000];

	if (recv(int(clientSocket), buffer, 10000, 0) < 0)
		utils::logging("Server: recv failed", 2);
	_request = new Request(buffer);
	_request->parseRequest();
//	print_rawRequest(std::string(buffer));
//	print_fullRequest(_request->getRequest());
	print_shortRequest(_request);
	utils::logging(_request->getMethod() + " " + _request->getPath()); //


	delete _request;
}

void	Server::responder()
{

}
