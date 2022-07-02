//
// Created by Grubbly Ernesto on 6/7/22.
//

#include "../hdrs/Request.hpp"

Request & Request::operator = (Request const &src)
{
	this->_rawRequest	= src._rawRequest;
	this->_request		= src._request;
	return *this;
}

Request::~Request()
{
	_request.clear();
}

void	Request::push_request(std::string requestPart)
{
	std::cout << requestPart;
	_rawRequest += requestPart;

	if (_rawRequest.find("\r\n\r\n") == std::string::npos)
	{
		_requestStatus = RE_INVALID;
		return;
	}

	std::vector<std::string>	request = split(_rawRequest, "\r\n\r\n");
	if (request.size() != 2)
		request.push_back("");

	if (!(_requestStatus & RE_HEAD))
		parseHeader(request.at(0));
	if (!((_requestStatus & RE_HEAD) & RE_BODY))
		parseBody(request.at(1));
}

void	Request::parseBody(std::string const &body)
{
	if (_request["Method"] == "GET")
	{
		std::vector<std::string>	params = split(_request["Path"], "?");

		if (params.size() == 2)
			_request["Body"] = params.at(1);
		_requestStatus |= RE_BODY;
		return;
	}
	if (!_transferEncoding)
	{
		if (!_request["Content-Length"].empty())
		{
			_contLength			= ::strtoul(_request["Content-Length"].c_str(), nullptr, 10);
			if (!_request["Content-Type"].empty() && _request["Content-Type"] == "multipart")
				_transferEncoding |= RE_MULTI;
			else
				_transferEncoding |= RE_SOLID;
		}
		else if (!_request["Transfer-Encoding"].empty() &&
				 _request["Transfer-Encoding"].find("chunked") != std::string::npos)
			_transferEncoding	|= RE_CHUNK;
		else
			_transferEncoding	|= RE_OTHER;
	}

	switch (_transferEncoding) {
		case RE_MULTI:
		case RE_SOLID:
			_request["Body"] += body;
			if (_request["Body"].size() != _contLength)
				return;
			break;
		case RE_CHUNK:
			std::cout << "CHUNKED" << std::endl;
			break;
		default:
			_requestStatus = RE_INVALID;
			return;
	}
	_requestStatus |= RE_BODY;
}

std::string	Request::getChunk(const std::string &src)
{
	std::string	chunk;



	return chunk;
}

void	Request::parseHeader(std::string const &header)
{
	std::vector<std::string>	params	= split(header, "\r\n");
	std::vector<std::string>	tmp;

	tmp = split(params.at(0));
	_request.insert(std::pair<std::string, std::string>("Method", tmp.at(0)));
	_request.insert(std::pair<std::string, std::string>("Path", tmp.at(1)));
	_request.insert(std::pair<std::string, std::string>("Protocol", tmp.at(2)));

	tmp = split(params.at(1), ":");
	_request.insert(std::pair<std::string, std::string>(tmp.at(0), trim(tmp.at(1)) + ":" + tmp.at(2))); // maybe remake

	for (u_int i = 2; i < params.size(); ++i)
	{
		tmp = split(params.at(i), ":");
		if (!tmp.at(0).empty() && tmp.size() == 2)
			_request.insert(std::pair<std::string, std::string>(tmp.at(0),trim(tmp.at(1))));
	}

	if (_request["Cookie"].empty())
		_request["Cookie"];
	if (_request["Content-Length"].empty())
		_request["Content-Length"];
	if (_request["Transfer-Encoding"].empty())
		_request["Transfer-Encoding"];
	if (_request["Body"].empty())
		_request["Body"];
	if (_request["Accept"].empty())
		_request["Accept"] = "*/*";
//	{
//		_request["Accept"] = _request["Accept-Encoding"];
//		_request.erase("Accept-Encoding");
//	}
//	_request["Accept"] = trim(split(_request.at("Accept"), ",").at(0));

	_requestStatus |= RE_HEAD;
}

void	Request::parseRequest()
{
	std::vector<std::string>	request	= split(_rawRequest, "\r\n\r\n");
	std::vector<std::string>	params	= split(request.at(0), "\r\n");
	std::vector<std::string>	tmp;

	tmp = split(params.at(0));
	_request.insert(std::pair<std::string, std::string>("Method", tmp.at(0)));
	_request.insert(std::pair<std::string, std::string>("Path", tmp.at(1)));
	_request.insert(std::pair<std::string, std::string>("Protocol", tmp.at(2)));

	tmp = split(params.at(1), ":");
	_request.insert(std::pair<std::string, std::string>(tmp.at(0), trim(tmp.at(1)) + ":" + tmp.at(2)));

	for (u_int i = 2; i < params.size(); ++i)
	{
		tmp = split(params.at(i), ":");
		if (!tmp.at(0).empty() && tmp.size() == 2)
			_request.insert(std::pair<std::string, std::string>(tmp.at(0),trim(tmp.at(1))));
	}

	if (_request["Accept"].empty())
	{
		_request["Accept"] = _request["Accept-Encoding"];
		_request.erase("Accept-Encoding");
	}
	_request["Accept"] = trim(split(_request.at("Accept"), ",").at(0));
	if (_request["Cookie"].empty())
		_request["Cookie"];
	if (request.size() == 1 || _request["Content-Length"].empty())
		_request["Body"] = std::string();
	else
		_request["Body"] = request.at(1).substr(0, std::stoi(_request.at("Content-Length")));
}
