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
//	static const std::string				_favicon;

private:
	std::string		_body;
	std::string		_header;
	std::string		_response;
	int				_statusCode;
	const Params	_config;
	const Request	*_request;

	std::string		_username;

	bool		is_autoindex();
	bool		is_redirect();
	bool		is_valid();
	void		craftHeader();
	void		craftResponse();

public:
	Response(Params const &, Request const *);
//	Response(Response const &other);
//	Response &operator = (Response const &other);
//	virtual ~Response();

	std::string	getResponse() const { return _response; };
	size_t		getRespLength() const { return _response.size() + 1; };

	void		process();

};


#endif //ZERO_WEBSERV_RESPONSE_HPP
