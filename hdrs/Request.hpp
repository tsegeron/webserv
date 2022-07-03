//
// Created by Grubbly Ernesto on 6/7/22.
//

#pragma once

# include "Utils.hpp"

# define RE_INVALID	0x0
# define RE_HEAD	0x1
# define RE_BODY	0x2
# define RE_TOTAL	0x3

# define RE_MULTI	0x1
# define RE_SOLID	0x2
# define RE_CHUNK	0x4
# define RE_OTHER	0x8

class Request {
private:
	std::string							_rawRequest;
	std::string							_head;
	std::string							_body;
	std::map<std::string, std::string>	_request;
	u_long								_contLength;
	int									_contRemaining;
	char								_requestStatus;
	char								_transferEncoding;

	bool	pushChunk(std::string const &);

public:
	Request() : _requestStatus(0x0), _transferEncoding(0x0) {};
	Request(std::string	const &raw) : _rawRequest(raw) {};
	Request(Request const &other) { *this = other; };
	Request &operator = (Request const &src);
	virtual ~Request();

	void	push_request(std::string);
	void	parseHeader(std::string const &);
	void	parseBody(std::string const &);

	void	parseRequest();

	bool								is_valid()			const { return !(_requestStatus ^ RE_TOTAL); }
	std::string							getRawRequest()		const { return _rawRequest; };
	std::map<std::string, std::string>	getRequest()		const { return _request; };
	std::string							getContLength()		const { return _request.at("Content-Length"); };
	std::string							getTransfEncoding()	const { return _request.at("Transfer-Encoding"); };
	std::string							getMethod()			const { return _request.at("Method"); };
	std::string							getPath()			const { return _request.at("Path"); };
	std::string							getCookie()			const { return _request.at("Cookie"); };
	std::string							getAccept()			const { return _request.at("Accept"); };
	std::string							getBody()			const { return _request.at("Body"); };
	std::string							getHost()			const { return utils::split(_request.at("Host"), ":")[0]; };
	std::string							getPort()			const { return utils::split(_request.at("Host"), ":")[1]; };
};
