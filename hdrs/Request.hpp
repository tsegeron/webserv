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
};


#endif //ZERO_WEBSERV_REQUEST_HPP
