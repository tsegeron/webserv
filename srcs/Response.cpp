//
// Created by Grubbly Ernesto on 6/13/22.
//

#include "../hdrs/Response.hpp"

const std::map<int, std::string>	Response::_statusPhrase = Response::_createMap();

std::map<int, std::string> Response::_createMap()
{
	std::map<int, std::string>	m;

	m[200] = "OK";
	m[301] = "Moved Permanently";
	m[304] = "Not Modified";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
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

void	Response::process()
{
	std::string	path = _request->getPath();

	if (path.find("bootstrap") != std::string::npos)
		_body = utils::readFile(utils::trim(path, "./"));
//	else if (path.find("favicon") != std::string::npos)
//		_body = utils::readFile("");
	else if (is_autoindex())
	{
//		execute cgi for autoindex
	}
	else if (is_redirect())
	{

	}
	else if (!is_valid())
	{
		_body = utils::readFile(_config.root + _config.error_pages_dir + std::to_string(_statusCode) + ".html");
		if (_body.empty())
			_body = utils::readFile("error_pages/" + std::to_string(_statusCode) + ".html");
	}
	else
	{
		_body = "<!DOCTYPE html>\n"
				"<html lang=\"en\">\n"
				"<head>\n"
				"<title>Home</title>\n"
				"</head>\n"
				"<body>\n"
				" <p>Hello</p>\n"
				"</body>\n"
				"</html>";
	}
	craftResponse();

	utils::logging(	_request->getMethod() + " " +
					_request->getPath() + " " +
					std::to_string(_statusCode) + " " +
					_statusPhrase.at(_statusCode) + " " +
					std::to_string(_body.size()));
}

bool	Response::is_redirect()
{
	std::string	location;

	try {
		if (!_config.locations.at(_request->getPath()).at("redirect").empty())
			return _statusCode = 301;
		return false;
	}
	catch (...) {
		return false;
	}
}

bool	Response::is_autoindex()
{
	try {
		if (_config.locations.at(_request->getPath()).at("autoindex") == "on")
			return true;
		else
			return false;
	}
	catch (...) {
		return false;
	}
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
		if (_request->getMethod() != "GET" && _request->getMethod() != "POST" && _request->getMethod() != "DELETE")
		{
			std::cout << "Here" << std::endl;
			std::cout << _request->getMethod() << std::endl;
			return !(_statusCode = 501);
		}
		if (confLocation.at("methods").find(_request->getMethod()) == std::string::npos)
			return !(_statusCode = 405);
	}
	catch (...) { return !(_statusCode = 405); }

//	checks if requested ...
//	_request->getCookie()
	return true;
}

void	Response::craftResponse()
{
	craftHeader();
	_response = _header + _body;
}

void	Response::craftHeader()
{
	_header = _request->getRequest()["Protocol"] + " " +
			  std::to_string(_statusCode) + " " +
			  _statusPhrase.at(_statusCode) + "\r\n" +
			  "Server: webserv\r\n" +
			  "Connection: Closed\r\n" +
			  "Content-Length: " + std::to_string(_body.size()) + "\r\n";
	if (_statusCode == 301)
	{
		std::map<std::string, std::string>	tmp = _request->getRequest();
		_header += std::string("Location: ") + "http://" + tmp.at("Host") + "/" +
				utils::trim(_config.locations.at(_request->getPath()).at("redirect"), "/") + "\r\n";
		std::cout << _header << std::endl;
	}
	_header += "Content-Type: " + _request->getAccept() + "\r\n\r\n";

}
