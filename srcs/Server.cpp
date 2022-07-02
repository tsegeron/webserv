#include "../hdrs/Server.hpp"


Server::Server(char **av)
{
	Config				config(av);
	std::vector<Params>	confParams;

	utils::createLogFile();
	if (!config.is_valid())
		throw ConfigException();

	if ((_kq = kqueue()) == -1)
	{
		utils::logging("kqueue(): " + std::string(strerror(errno)));
		throw KqueueException();
	}
	confParams	= config.getParams();
	for (u_int i = 0; i < confParams.size(); ++i)
	{
		_sockets.push_back(new SimpSocket(confParams[i].host, confParams[i].port));
        utils::logging( "Server #" + std::to_string(i + 1) +
                        " started on http://" + confParams[i].host + ":" +
						std::to_string(confParams[i].port), utils::serverInfo);
		SimpSocket	*tmp = *(--_sockets.end());
		if (!tmp->setSocketAsNonblock() || !tmp->allowMultipleConnectionsOnSocket() ||
			!tmp->bindSocketToLocalSockaddr() || !tmp->listenForConnectionsOnSocket())
			throw SocketException();
//		_requests[tmp->getServerFd()];
		_configs.insert(std::pair<int, Params>(tmp->getServerFd(), confParams[i]));

		updateEvent(tmp->getServerFd(), EVFILT_READ, EV_ADD, 0, 0, nullptr);
	}
//	_evList.resize(confParams.size());
}

Server::Server(Server const &src)
{
	*this = src;
}

Server &Server::operator = (const Server &src)
{
	this->_sockets			= src._sockets;
	this->_requests			= src._requests;
	this->_configs			= src._configs;
	this->_timeout			= src._timeout;

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
	int		numberOfEvents;

	while (true)
	{
		if ((numberOfEvents = kevent()) <= 0)
			continue;
		accept(numberOfEvents);
//		handle();
	}
}

int		Server::kevent()
{
	std::string			dots[4] = {"", ".", "..", "..."};
	int					eventsNum = 0, n = -1;

//	while (eventsNum == 0)
//	{
//		std::cout << YELLOW << "\33[2K\rWaiting for connection" << dots[++n] << RESET << std::flush;
//		if (n == 3)
//			n = -1;
//	eventsNum = ::kevent(_kq, nullptr, 0, &*_evList.begin(), int(_evList.size()), &_timeout);
	eventsNum = ::kevent(_kq, nullptr, 0, _test, 1024, &_timeout);
//	}
//	std::cout << "\33[2K\r"; // cleans the terminal line

//	if (eventsNum == -1)
//	{
//		utils::logging(strerror(errno), utils::error);
//		throw KeventException();
//	}

	return eventsNum;
}


void	Server::accept(int numberOfEvents)
{
	Client	*client = nullptr;

	for (int i = 0; i < numberOfEvents; ++i)
	{
		if (_configs.find((int)_test[i].ident) != _configs.end())
			acceptConnection((int)_test[i].ident);
		else
		{
			Clients::iterator it = _clients.find((int)_test[i].ident);
			if (it != _clients.end())
				client = it->second;

			if (client == nullptr)
			{
				utils::logging("Client not found", utils::serverInfo);
				updateEvent((int)_test[i].ident, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
				updateEvent((int)_test[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);
				close((int)_test[i].ident);
				continue;
			}
			else
			{
				if (_test[i].flags & EV_ERROR) // manage errors here
				{
					std::cout << "EV_ERROR" << std::endl;
					continue; // set status code to 50*
				}
				if (_test[i].flags & EV_EOF) // client disconnects
				{
//					std::cout << "EV_EOF: " << _test[i].ident << std::endl;
					dropConnection(client);
					continue;
				}

				if (_test[i].filter == EVFILT_READ && receiver(client, _test[i].data))
				{
//					std::cout << "READ: " << _test[i].ident << std::endl;

					updateEvent((int)_test[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, nullptr);
					updateEvent((int)_test[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, nullptr);
				}
				else if (_test[i].filter == EVFILT_WRITE && !sender(client, _test[i].data))
//				else if (_test[i].filter == EVFILT_WRITE && !sender_test(client, _configs[client->getSocketFd()].body_size_limit))
				{
					updateEvent((int) _test[i].ident, EVFILT_READ, EV_ENABLE, 0, 0, nullptr);
					updateEvent((int) _test[i].ident, EVFILT_WRITE, EV_DISABLE, 0, 0, nullptr);
				}
			}
		}
	}
}

bool	Server::sender(Client *cl, size_t availBytes)
{
//	int				caseId;
	size_t			actual_sent;
	size_t			attempt_to_send;
	size_t			remaining;
	static size_t	total_sent;

	std::string	host	= _requests[cl->getAcceptFd()]->getHost();
	std::string	port	= _requests[cl->getAcceptFd()]->getPort();
	size_t		i		= 0;
	for (; i < _configs.size(); ++i)
		if (_configs[int(i)].host == host && std::to_string(_configs[int(i)].port) == port)
			break ;
	Response	response(_configs[int(i)], _requests[cl->getAcceptFd()]);
	response.process();
//	caseId = response.process();

	remaining = response.getRespLength() - total_sent;
//	if (caseId == Response::Action::bootstrap)
//		attempt_to_send = resp.size() + 1;
//	else if (availBytes >= remaining)
	if (availBytes >= remaining)
		attempt_to_send = remaining;
	else
		attempt_to_send = availBytes;

	actual_sent	= send(cl->getAcceptFd(), total_sent + response.getResponse().c_str(), attempt_to_send, 0);
//	std::cout << actual_sent << std::endl;
	total_sent += actual_sent;

	if (total_sent - response.getHeaderLength() >= availBytes)
	{
		total_sent = 0;
		delete _requests[cl->getAcceptFd()];
		_requests.erase(_requests.find(cl->getAcceptFd()));
		dropConnection(cl);
	}

	if (total_sent >= response.getRespLength())
	{
//		std::cout << response.getResponse() << std::endl;
//		std::cout	<< "SEND:	[" << cl->getAcceptFd()	<< "]; "
//					<< "TOTAL:	[" << total_sent		<< "]; "
//					<< "REQ:	[" << response.getRespLength()	<< "]"
//					<< std::endl;
		total_sent = 0;
		delete _requests[cl->getAcceptFd()];
		_requests.erase(_requests.find(cl->getAcceptFd()));
		return false;
	}
	return true;
}

bool	Server::receiver(Client *cl, long dataLen)
{
//	std::cout << dataLen << std::endl;
	if (cl == nullptr)
		return false;
	if (!dataLen)
		dataLen = 1500; // MTU by default

	char				buffer[dataLen];
	size_t				bytes;

	bzero(buffer, dataLen);
	if ((bytes = ::recv(cl->getAcceptFd(), buffer, dataLen, 0)) == -1)
	{
		utils::logging("Server: recv failed", utils::error);
		dropConnection(cl);
		return false;
	}
	else if (!bytes)
	{
		utils::logging("Client " + cl->getIp() + " wished to take his leave", utils::serverInfo);
		dropConnection(cl);
		return false;
	}

	if (_requests.find(cl->getAcceptFd()) == _requests.end())
		_requests[cl->getAcceptFd()] = new Request();

	_requests[cl->getAcceptFd()]->push_request(std::string(buffer, bytes));

	if (!_requests[cl->getAcceptFd()]->is_valid())
		return false;

//	utils::print_rawRequest(_requests[cl->getAcceptFd()]->getRawRequest());
//	utils::print_shortRequest(_requests[cl->getAcceptFd()]);
//	utils::print_fullRequest(_requests[cl->getAcceptFd()]->getRequest());

	return true;
}

void	Server::acceptConnection(int socketFd)
{
	sockaddr_in	clientAddress;
	socklen_t	clientAddressLen = sizeof clientAddress;
	int			clientFd;

	if ((clientFd = ::accept(socketFd, (struct sockaddr *)&clientAddress, &clientAddressLen)) == -1)
	{
		utils::logging("Error: accept()", utils::error); // http code 500
		return;
	}
	fcntl(clientFd, F_SETFL, O_NONBLOCK);

	std::string	ip = _configs[socketFd].host + ":" + std::to_string(_configs[socketFd].port);
	_clients.insert(std::pair<int, Client *>(clientFd, new Client(socketFd, ip, clientFd, clientAddress)));

	updateEvent(clientFd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, nullptr);
	updateEvent(clientFd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, nullptr);

	utils::logging("New connection on " + _clients[clientFd]->getIp() + " [" + std::to_string(clientFd) + "]", utils::serverInfo);
}

void	Server::updateEvent(int socketFD, short filter, ushort flags, uint fflags, int data, void *udata, bool add)
{
	struct kevent ke;
	struct timespec	timeout = { 10, 0 };

	EV_SET(&ke, socketFD, filter, flags, fflags, data, udata);

//	if (add)
//		_chList.push_back(ke);

	if (::kevent(_kq, &ke, 1, nullptr, 0, &timeout) == -1)
		throw KeventException();
}

void	Server::dropConnection(Client *cl, bool erase)
{
	if (cl == nullptr)
		return;

	updateEvent(cl->getAcceptFd(), EVFILT_READ, EV_DELETE, 0, 0, nullptr);
	updateEvent(cl->getAcceptFd(), EVFILT_WRITE, EV_DELETE, 0, 0, nullptr);

	close(cl->getAcceptFd());

	if (erase)
	{
//		delete _clients.at(cl->getAcceptFd());
		_clients.erase(_clients.find(cl->getAcceptFd()));
	}
//	if (_requests.find(cl->getAcceptFd()) != _requests.end())
//		delete _requests[cl->getAcceptFd()];

	utils::logging("Client " + cl->getIp() + " disconnected [" + std::to_string(cl->getAcceptFd()) + "]", utils::serverInfo);

	delete cl;
}






bool	Server::sender_test(Client *cl, size_t availBytes)
{
	if (cl == nullptr)
		return false;
	static	size_t	total_sent;

	size_t	actual_sent		= 0;	// Actual number of bytes sent as returned by send()
	size_t	attempt_sent	= 0;	// Bytes that we're attempting to send now
	size_t	remaining		= 0;	// Size of data left to send for the item
	bool	disconnect		= false;
	u_int	i				= 0;

	if (availBytes > 1500)
		availBytes = 1500;
	else if (!availBytes)
		availBytes = 64;

	std::cout << "SEND: " << cl->getAcceptFd() << std::endl;

	std::string	host	= _requests[cl->getAcceptFd()]->getHost();
	std::string	port	= _requests[cl->getAcceptFd()]->getPort();
	for (; i < _configs.size(); ++i)
		if (_configs[int(i)].host == host && std::to_string(_configs[int(i)].port) == port)
			break ;
	Response	response(_configs[int(i)], _requests[cl->getAcceptFd()]);
	response.process();

	remaining	= response.getRespLength() - total_sent;
	if (availBytes >= remaining)
		attempt_sent = remaining;
	else
		attempt_sent = availBytes;

	actual_sent	= send(cl->getAcceptFd(), total_sent + response.getResponse().c_str(), attempt_sent, 0);
	std::cout << actual_sent << std::endl;
	total_sent += actual_sent;

	if (actual_sent < 0)
	{
		total_sent = 0;
		std::cout << "DROPPED" << std::endl;
		delete _requests[cl->getAcceptFd()];
		dropConnection(cl);
		return false;
	}
	if (total_sent >= response.getRespLength())
	{
		std::cout << "TOTAL: [" << total_sent << "] RESP_LENGTH: [" << response.getRespLength() << "]" << std::endl;
		total_sent = 0;
		delete _requests[cl->getAcceptFd()];
		return false;
	}

	return true;
}