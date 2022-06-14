//
// Created by Grubbly Ernesto on 6/13/22.
//

#ifndef ZERO_WEBSERV_RESPONSE_HPP
# define ZERO_WEBSERV_RESPONSE_HPP

# include "Request.hpp"
# include "Utils.hpp"

class Response {
private:
	static std::map<int, std::string>		_createMap();
	static const std::map<int, std::string> _statusPhrase;

	std::string								_header;
	int										_statusCode;
	int										_socket;
	const Request							*_request;

public:
	Response(int, Request const *);
	Response(Response const &other);
	Response &operator = (Response const &other);
	virtual ~Response();

	std::string	craftHeader(int, std::string const &) const;

};


#endif //ZERO_WEBSERV_RESPONSE_HPP
