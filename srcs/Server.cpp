#include "../hdrs/Server.hpp"

typedef typename std::vector<std::pair<std::string, int>>	HostsPortsPair;

Server::Server(char **av)
{
	HostsPortsPair	hostsPortsPair;

	utils::createLogFile();
	_config		= new Config(av);
	if (!_config->is_valid())
		throw ConfigException();

	hostsPortsPair = _config->getHostsPortsPair();
	for (u_int i = 0; i < hostsPortsPair.size(); ++i)
	{
		_servSockets.push_back(new SimpSocket(hostsPortsPair[i].first, hostsPortsPair[i].second));
		if (!_servSockets[i]->is_socket_valid() || !_servSockets[i]->allowMultipleConnectionsOnSocket() ||
			!_servSockets[i]->bindSocketToLocalSockaddr() || !_servSockets[i]->listenForConnectionsOnSocket(BACKLOG))
			throw SocketException();
		_requests[_servSockets[i]->getServerFd()];
		_fds.push_back(_servSockets[i]->getServerFd());
	}

	_timeout.tv_sec = TIMEOUT;
	_timeout.tv_usec = 0;
	FD_ZERO(&_currentSockets);
	FD_ZERO(&_readSockets);
	FD_ZERO(&_writeSockets);
	for (u_int i = 0; i < _servSockets.size(); ++i)
		FD_SET(_servSockets[i]->getServerFd(), &_currentSockets);
}

Server::Server(Server const &src)
{
	*this = src;
}

Server &Server::operator = (const Server &src)
{
	this->_servSockets		= src._servSockets;
	this->_currentSockets	= src._currentSockets;
	this->_readSockets		= src._readSockets;

	return *this;
}

Server::~Server()
{
	delete	_config;
	_config	= nullptr;

	for (u_int i = 0; i < _servSockets.size(); ++i)
	{
		delete _servSockets[i];
		_servSockets[i] = nullptr;
	}
	for (Requests::iterator it = _requests.begin(); it != _requests.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
	_servSockets.clear();
	_requests.clear();
}

void	Server::runServer()
{
	struct sockaddr_in	address	= _servSockets[0]->getAddress();
	int					addrLen	= _servSockets[0]->getAddressLen();

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
		std::cout << YELLOW << "\33[2K\rWaiting for connection" << dots[++n] << RESET << std::flush;
		if (n == 3)
			n = -1;
		_readSockets = _currentSockets;
		FD_ZERO(&_writeSockets);
		selectStat = select(FD_SETSIZE, &_readSockets, &_writeSockets, nullptr, &timeout); // max num of sockets' fds (FD_SETSIZE)
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
				if (std::find(_fds.begin(), _fds.end(), i) != _fds.end())
				{
					socket = accept(i, (struct sockaddr *)&address, (socklen_t *)&addrLen);
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

void	print_fullRequest(std::map<std::string, std::string> const &request)
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

void	Server::handler(int clientSocket)
{
	char		buffer[10000];

	if (recv(clientSocket, buffer, 10000, 0) == -1)
		utils::logging("Server: recv failed", 2);

	_requests[clientSocket] = new Request(std::string(buffer));
	_requests[clientSocket]->parseRequest();
//	print_fullRequest(_requests[clientSocket]->getRequest());
	print_shortRequest(_requests[clientSocket]);
	utils::logging(_requests[clientSocket]->getMethod() + " " + _requests[clientSocket]->getPath());

	delete _requests[clientSocket];
}

void	Server::responder()
{

}
