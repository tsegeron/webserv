//
// Created by Grubbly Ernesto on 6/7/22.
//

#ifndef ZERO_WEBSERV_REQUEST_HPP
# define ZERO_WEBSERV_REQUEST_HPP

# include "Utils.hpp"
# include <map>

class Request {
private:
	std::string							_rawRequest;
	std::map<std::string, std::string>	_request;

public:
	Request(std::string	const &raw) : _rawRequest(raw) {};
	Request(Request const &other) { *this = other; };
	Request &operator = (Request const &src);
	virtual ~Request();

	void	parseRequest();

	std::map<std::string, std::string>	getRequest() const { return _request; };
	std::string							getMethod() const { return _request.at("Method"); };
	std::string							getPath() const { return _request.at("Path"); };
	std::string							getCookie() const { return _request.at("Cookie"); };
	std::string							getAccept() const { return _request.at("Accept"); };
	std::string							getBody() const { return _request.at("Body"); };
	std::string							getHost() const { return utils::split(_request.at("Host"), ":")[0]; };
	std::string							getPort() const { return utils::split(_request.at("Host"), ":")[1]; };
};


#endif //ZERO_WEBSERV_REQUEST_HPP
