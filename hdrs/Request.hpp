//
// Created by Grubbly Ernesto on 6/7/22.
//

#pragma once

# include "Utils.hpp"
# include "ConfigParams.hpp"

# define REQ_INVALID	0x0
# define REQ_HEAD		0x1
# define REQ_BODY		0x2
# define REQ_TOTAL		0x3

# define REQ_MULTI	0x1
# define REQ_SOLID	0x2
# define REQ_CHUNK	0x4
# define REQ_OTHER	0x8

class Request {
private:
	std::map<int, Params>				*_configs;
	std::string							_rawRequest;
	std::string							_head;
	std::string							_body;
	std::map<std::string, std::string>	_request;
	u_long								_contLength;
//	int									_contRemaining;
	char								_requestStatus;
	char								_transferEncoding;
    std::string                         sep;

    bool	pushChunk(std::string const &);

public:
//	Request() : _requestStatus(0x0), _transferEncoding(0x0) {};
	Request(std::map<int, Params> *configs) : _configs(configs), _requestStatus(0x0), _transferEncoding(0x0) {};
//	Request(std::string	const &raw) : _rawRequest(raw) {};
	Request(Request const &other) { *this = other; };
	Request &operator = (Request const &src);
	virtual ~Request();

	void	push_request(std::string const &);
	void	parseHeader(std::string const &);
	void	parseBody(std::string const &);

    void    downloadFile(std::string const &body);

	bool								is_valid()			const { return !(_requestStatus ^ REQ_TOTAL); }
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
