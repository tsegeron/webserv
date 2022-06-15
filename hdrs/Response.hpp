//
// Created by Grubbly Ernesto on 6/13/22.
//

#ifndef ZERO_WEBSERV_RESPONSE_HPP
# define ZERO_WEBSERV_RESPONSE_HPP

# include "Request.hpp"
# include "Config.hpp"

class Response {
	static std::map<int, std::string>		_createMap();
	static const std::map<int, std::string> _statusPhrase;

private:
	std::string		_response;
	std::string		_header;
	int				_statusCode;
	const Params	_config;
	const Request	*_request;

	std::string		_username;

	bool		is_valid();

public:
	Response(Params const &, Request const *);
//	Response(Response const &other);
//	Response &operator = (Response const &other);
//	virtual ~Response();

	std::string	getResponse() const { return craftHeader() + _response; };
	size_t		getRespLength() const { return _response.size() + 1; };

	std::string	craftHeader() const;
	void		process();

};


#endif //ZERO_WEBSERV_RESPONSE_HPP
