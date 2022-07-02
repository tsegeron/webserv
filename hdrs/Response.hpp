//
// Created by Grubbly Ernesto on 6/13/22.
//

#pragma once

# include "Request.hpp"
# include "Config.hpp"

class Response {
	static std::map<int, std::string>		_createMap();
	static const std::map<int, std::string> _statusPhrase;
//	static const std::string				_favicon;

public:
	enum	Action {
		invalid = 1,
		bootstrap,
		autoindexation,
		redirection,
		cgi,
		other
	};

private:
	std::string		_body;
	std::string		_header;
	int				_statusCode;
	Params			_config;
	Request			*_request;

	std::string		_response;

	std::string		_username;

	int			action_to_do(std::string &, int *);
	std::string	is_autoindex();
	bool		is_redirect();
	bool		is_cgi();
	bool		is_valid();
	void		craftHeader();
	void		craftResponse();
	std::string	getCgiResponse(const std::string &scriptName, std::string const &filename = "") const;
	char		**getCgiEnv(std::string const &scriptName, std::string const &filename = "") const;

public:
	Response(Params &, Request *);
//	Response(Response const &other);
//	Response &operator = (Response const &other);
//	virtual ~Response();

	std::string	getResponse()		const { return _response; };
	size_t		getRespLength()		const { return _response.size() + 1; };
	size_t		getHeaderLength()	const { return _header.size(); };
	size_t		getBodyLength()		const { return _header.size(); };

	int			process();

};
