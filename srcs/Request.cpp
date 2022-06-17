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

void	Request::parseRequest()
{
	std::vector<std::string>	request	= utils::split(_rawRequest, "\r\n\r\n");
	std::vector<std::string>	params	= utils::split(request.at(0), "\r\n");
	std::vector<std::string>	tmp;

	tmp = utils::split(params.at(0));
	_request.insert(std::pair<std::string, std::string>("Method", tmp.at(0)));
	_request.insert(std::pair<std::string, std::string>("Path", tmp.at(1)));
	_request.insert(std::pair<std::string, std::string>("Protocol", tmp.at(2)));

	tmp = utils::split(params.at(1), ":");
	_request.insert(std::pair<std::string, std::string>(tmp.at(0), utils::trim(tmp.at(1)) + ":" + tmp.at(2)));

	for (int i = 2; i < params.size(); ++i)
	{
		tmp = utils::split(params.at(i), ":");
		if (!tmp.at(0).empty())
			_request.insert(std::pair<std::string, std::string>(tmp.at(0),utils::trim(tmp.at(1))));
	}

	_request["Accept"] = utils::trim(utils::split(_request.at("Accept"), ",").at(0));
	if (_request["Cookie"].empty())
		_request["Cookie"];
	if (request.size() == 1)
		_request["Body"] = std::string();
	else
		_request["Body"] = utils::trim(request.at(1));
}
