//
// Created by Grubbly Ernesto on 6/13/22.
//

#include "../hdrs/Response.hpp"

const std::map<int, std::string>	Response::_statusPhrase = Response::_createMap();

std::map<int, std::string> Response::_createMap()
{
	std::map<int, std::string>	m;

	m[200] = "OK";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[500] = "Server Error";
	m[501] = "Not Implemented";
	m[503] = "Out of Resources";
	m[504] = "Gateway Time-Out";

	return m;
}

Response::Response(int fd, const Request *request)
	: _socket(fd), _request(request), _statusCode(200)
{
	_request->getRequest()["Host"]
}

std::string	Response::craftHeader(int contLength, std::string const &contType) const
{
	return	_request->getRequest()["Protocol"] + " " +
			std::to_string(_statusCode) + " " +
			_statusPhrase.at(_statusCode) + "\r\n" +
			"Content-Length: " + "88" + "\r\n" +
			"Content-Type: text/html\r\n\r\n";
}

int main()
{

}
