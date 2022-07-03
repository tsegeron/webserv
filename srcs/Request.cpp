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
	std::vector<std::string>	request;

	std::cout << requestPart;
	_rawRequest += requestPart;

	if (!(_requestStatus & RE_HEAD) && _rawRequest.find("\r\n\r\n") == std::string::npos)
	{
		_requestStatus = RE_INVALID;
		return;
	}

	if (!(_requestStatus & RE_HEAD))
	{
		request = split(_rawRequest, "\r\n\r\n", 1);
		if (request.size() != 2)
			request.push_back("");
	}

	if (!(_requestStatus & RE_HEAD))
		parseHeader(request.at(0));

	if (!((_requestStatus & RE_HEAD) & RE_BODY))
	{
		if (request.empty())
			parseBody(requestPart);
		else
			parseBody(request.at(1));
	}
}

void	Request::parseBody(std::string const &body)
{
	if (_request["Method"] == "GET" || _request["Method"] == "HEAD")
	{
		std::vector<std::string>	params = split(_request["Path"], "?", 1);

		if (params.size() == 2)
			_request["Body"] = params.at(1);
		else
			_request["Body"] = "";
		_requestStatus |= RE_BODY;
		return;
	}
	if (body.empty())
		return;

	switch (_transferEncoding) {
		case RE_MULTI:
		case RE_SOLID:
			_request["Body"] += body;
			if (_request["Body"].size() != _contLength)
				return;
			break;
		case RE_CHUNK:
			std::cout << "CHUNKED" << std::endl;
			if (!pushChunk(body))
				return;
			break;
		default:
			_requestStatus = RE_INVALID;
			return;
	}
	_requestStatus |= RE_BODY;
}

bool	Request::pushChunk(std::string const &chunk)
{
	std::vector<std::string>	splitted = split(chunk, "\r\n", 2);

	if (splitted.size() == 1)
	{
		if (splitted.at(0) == "0")
		{
			_request["Body"] += "";
			_requestStatus
		}
		else
			_requestStatus = RE_INVALID;
		return false;
	}
	else if (splitted.size() == 2)
	{

	}

	return true;
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
		else if (!_request["Transfer-Encoding"].empty() && (_request["Content-Length"].empty() ||
				 _request["Transfer-Encoding"].find("chunked") != std::string::npos))
			_transferEncoding	|= RE_CHUNK;
		else
			_transferEncoding	|= RE_OTHER;
	}

	_requestStatus |= RE_HEAD;
	_head			= header;
}
