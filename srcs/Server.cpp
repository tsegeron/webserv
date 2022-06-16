#include "../hdrs/Server.hpp"


Server::Server(char **av)
{
	Config				config(av);
	std::vector<Params>	confParams;

	utils::createLogFile();
	if (!config.is_valid())
		throw ConfigException();

	confParams = config.getParams();
	for (u_int i = 0; i < confParams.size(); ++i)
	{
		_sockets.push_back(new SimpSocket(confParams[i].host, confParams[i].port));
		SimpSocket	*tmp = *(--_sockets.end());
		if (!tmp->is_socket_valid() || !tmp->allowMultipleConnectionsOnSocket() ||
			!tmp->bindSocketToLocalSockaddr() || !tmp->listenForConnectionsOnSocket(BACKLOG))
			throw SocketException();
		_requests[tmp->getServerFd()];
		_fds.insert(std::pair<int, Params>(tmp->getServerFd(), confParams[i]));
	}

	_timeout.tv_sec = TIMEOUT;
	_timeout.tv_usec = 0;
	FD_ZERO(&_currentSockets);
	FD_ZERO(&_readSockets);
	FD_ZERO(&_writeSockets);
	for (u_int i = 0; i < _sockets.size(); ++i)
		FD_SET(_sockets[i]->getServerFd(), &_currentSockets);
}

Server::Server(Server const &src)
{
	*this = src;
}

Server &Server::operator = (const Server &src)
{
	this->_sockets			= src._sockets;
	this->_currentSockets	= src._currentSockets;
	this->_readSockets		= src._readSockets;

	return *this;
}

Server::~Server()
{
	for (u_int i = 0; i < _sockets.size(); ++i)
	{
		delete _sockets[i];
		_sockets[i] = nullptr;
	}
	_sockets.clear();
	_requests.clear();
}

void	Server::runServer()
{
	struct sockaddr_in	address	= _sockets[0]->getAddress();
	int					addrLen	= _sockets[0]->getAddressLen();

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
		utils::logging(strerror(errno), 2);
	else
	{
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET(i, &_readSockets))
			{
				if (_fds.find(i) != _fds.end())
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
//	utils::logging(_requests[clientSocket]->getMethod() + " " + _requests[clientSocket]->getPath());
//	print_rawRequest(std::string(buffer));
//	print_fullRequest(_requests[clientSocket]->getRequest());
//	print_shortRequest(_requests[clientSocket]);


	int			i		= 0;
	std::string	host	= _requests[clientSocket]->getHost();
	std::string	port	= _requests[clientSocket]->getPort();
	for (; i < _fds.size(); ++i)
		if (_fds[i].host == host && std::to_string(_fds[i].port) == port)
			break ;
	Response	response(_fds[i], _requests[clientSocket]);
	response.process();
//	std::cout << response.getResponse() << std::endl;
//	std::cout << response.getRespLength() << std::endl;
	send(clientSocket, response.getResponse().c_str(), response.getRespLength(), 0);
//	std::string data = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" /><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /><title>405 Method Not Allowed</title></head><body><div class=\"d-flex bg-secondary align-items-center justify-content-center vh-100\"><div class=\"text-center text-light\"><h1 class=\"display-1 fw-bold\">405</h1><p class=\"lead text-white-50 fs-2\">Method Not Allowed</p><p class=\"text-white-50 fs-5\">Method Not Allowed.</p><a class=\"btn btn-outline-dark\" href=\"/\" role=\"button\">Home</a></div></div></body></html>";
//	std::string tmp = "HTTP/1.1 405 Method Not Allowed\r\n"
//					  "Server: webserv\r\n"
//					  "Content-Length: " + std::to_string(data.size()) + "\r\n"
//					  "Connection: Closed\r\n"
//					  "Content-Type: text/html\r\n\r\n";
//	send(clientSocket, (tmp + data).c_str(), (tmp + data).size() + 1, 0);

	delete _requests[clientSocket];
}

//void	Server::responder(int clientSocket, Request const *request)
//{
//	std::string	response = utils::readFile("error_pages/404.html");
//	send(clientSocket, response.c_str(), response.size() + 1, 0);
//}
