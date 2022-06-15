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

Response::Response(Params const &config, Request const *request)
	: _config(config), _request(request), _statusCode(200)
{
}

bool	Response::is_valid()
{
	std::map<std::string, std::string>	confLocation;

//	checks if requested PATH exists
	try {
		confLocation = _config.locations.at(_request->getPath());
	}
	catch (...) { return !(_statusCode = 404); }

//	checks if requested METHOD allowed
	try {
		if (_request->getMethod() != "GET" || _request->getMethod() != "POST" || _request->getMethod() != "DELETE")
			return !(_statusCode = 501);
		if (confLocation.at("methods").find(_request->getMethod()) == std::string::npos)
			return !(_statusCode = 405);
	}
	catch (...) { return !(_statusCode = 405); }

//	checks if requested
//	_request->getCookie()
	return true;
}

void	Response::process()
{
	std::string	bootstrapPath = _request->getPath();

	if (bootstrapPath.find("bootstrap5") != std::string::npos)
	{
		_response = utils::readFile(bootstrapPath);
		_statusCode = 200;
		return;
	}
	if (!is_valid())
	{
		_response = utils::readFile(_config.root + _config.error_pages_dir + std::to_string(_statusCode) + ".html");
		if(_response.empty())
			_response = utils::readFile("error_pages/" + std::to_string(_statusCode) + ".html");
		return;
	}

}

std::string	Response::craftHeader() const
{
	return	_request->getRequest()["Protocol"] + " " +
			std::to_string(_statusCode) + " " +
			_statusPhrase.at(_statusCode) + "\r\n" +
			"Content-Length: " + std::to_string(_response.size()) + "\r\n" +
			"Content-Type: " + _request->getAccept() + "\r\n\r\n";
}

//int main()
//{
//
//}
