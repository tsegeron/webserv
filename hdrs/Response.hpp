//
// Created by Grubbly Ernesto on 6/13/22.
//

#pragma once

# include "Request.hpp"
# include "Config.hpp"

# define RES_INVALID	0x0
# define RES_PUT		0x1
# define RES_DELETE 	0x2

class Response {
	static std::map<int, std::string>			_createStatusPhrasesMap();
	static const std::map<int, std::string> 	_statusPhrase;
	static const std::string					_imageMIMETypes;
//	static const std::string					_favicon;

public:
	enum	Action {
		invalid = 1,
        image,
		bootstrap,
		autoindexation,
		redirection,
		cgi_loc,
		cgi_com,
		file_modif,
		page
	};

private:
	std::string		_body;
	std::string		_header;
	int				_statusCode;
	Params			_config;
	Request			*_request;

	std::string		_response;
	int				_caseNum;

//	std::string		_username;

	int			action_to_do(std::string &);
	bool		is_body_slim();
	bool		is_image();
	bool		is_redirect(std::string const &);
	bool		is_autoindex(std::string const &);
	bool		is_method_alowed(std::string const &);
	bool		is_cgi(std::string const &);
	std::string	is_file_modification(std::string const &);
	std::string	is_page(std::string const &);
	void		craftHeader();
	void		craftResponse();
	bool		handleModification(std::string const &);
	void        getCgiResponse(const std::string &scriptName, std::string const &filename = "");
	char		**getCgiEnv(std::string const &scriptName, std::string const &filename = "") const;
    bool		getImageBytes();

public:
	Response(Params &, Request *);
//	Response(Response const &other);
//	Response &operator = (Response const &other);
//	virtual ~Response();

	int			getStatusCode()		const { return _statusCode; };
	std::string	getStatusPhrase()	const { return _statusPhrase.at(_statusCode); };
	std::string	getResponse()		const { return _response; };
	size_t		getRespLength()		const { return _response.size(); };
	size_t		getHeaderLength()	const { return _header.size(); };
	size_t		getBodyLength()		const { return _body.size(); };

	int			process();

};
