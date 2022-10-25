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

void	Request::push_request(std::string const &requestPart)
{
	std::vector<std::string>	request;

//	std::cout << requestPart;
	_rawRequest += requestPart;

	if (!(_requestStatus & REQ_HEAD) && _rawRequest.find("\r\n\r\n") == std::string::npos)
	{
		_requestStatus = REQ_INVALID;
		return;
	}

	if (!(_requestStatus & REQ_HEAD) || _transferEncoding & REQ_MULTI)
	{
		request = split(_rawRequest, "\r\n\r\n", 1);
		if (request.size() != 2)
			request.push_back("");
	}

	if (!(_requestStatus & REQ_HEAD))
		parseHeader(request.at(0));

	if (!((_requestStatus & REQ_HEAD) & REQ_BODY))
	{
		if (request.empty())
			parseBody(requestPart);
		else
			parseBody(request.at(1));
	}
}

void Request::downloadFile(std::string const &body) {
    std::vector<std::string>	form_data_file;
    std::vector<std::string>	form_data_desc;
    std::ofstream				fout;
    std::string					filename;

	std::string	host	= this->getHost();
	std::string	port	= this->getPort();
	size_t		i		= 0;
	for (; i < _configs->size(); ++i)
		if (std::to_string((*_configs)[int(i)].port) == port)
			break ;

    form_data_file = split(body, "--" + sep);
	filename = trim((*_configs)[int(i)].locations[this->getPath()].at("storage"), "/") + "/" + utils::getCurrentTime() +
			   trim(split(split(form_data_file.at(0), "\r\n", 1).at(0), "filename=").at(1), "\"");
    form_data_desc = split(form_data_file.at(1), "\r\n");
    form_data_file = split(form_data_file.at(0), "\r\n", 3);

    fout.open(filename);
//    fout << form_data_file.at(2);
//    if (form_data_file.size() == 4)
//        fout << "\r\n";
    for (i = 2; i < form_data_file.size(); i++)
        fout << form_data_file.at(i) + "\r\n";
//        fout << trim(form_data_file.at(i), "\r\n");
    fout.close();
    _request["Body"] = "img_path=" + filename + "&" + "description=";
    if (form_data_desc.size() > 1)
        _request["Body"] += form_data_desc.at(1);
}

void	Request::parseBody(std::string const &body)
{
    std::vector<std::string>	form_data;

	if (_request["Method"] != "PUT" && _request["Method"] != "POST" &&
		_request["Method"] != "GET" && _request["Method"] != "HEAD" && _request["Method"] != "DELETE")
	{
		_requestStatus |= REQ_BODY;
		return;
	}

	if (_request["Method"] == "GET" || _request["Method"] == "HEAD" || _request["Method"] == "DELETE")
	{
		_requestStatus |= REQ_BODY;
		return;
	}
	if (body.empty())
		return;
	switch (_transferEncoding) {
		case REQ_MULTI:
            sep = trim(split(_request["Content-Type"], "boundary=").at(1), "\r\n");
            form_data = split(body, "--" + sep);
            if (form_data.empty())
            {
                _requestStatus = REQ_INVALID;
                return;
            }
            if (form_data.at(form_data.size() - 1) != "--\r\n")
                return;
            downloadFile(body);
            break;
        case REQ_SOLID:
			if (!_request["Body"].empty())
				_request["Body"] += "&";
			_request["Body"] += body;
//			if (_request["Body"].size() < _contLength)
//				return;
			break;
		case REQ_CHUNK:
			if (!pushChunk(body))
				return;
			break;
		default:
			_requestStatus = REQ_INVALID;
			return;
	}
	_requestStatus |= REQ_BODY;

}

bool	Request::pushChunk(std::string const &chunk)
{
	static std::string	left;

	left += chunk;
	if (left.find("0\r\n\r\n") == std::string::npos)
		return false;

	std::vector<std::string>	splitted = split_chunk(left);

	for (size_t i = 0; i < splitted.size(); i += 2)
	{
		if (splitted.at(i) == "0")
		{
			_request["Body"] += "";
			left = "";
			return true;
		}

		long		chunk_size	= ::strtol(split(splitted[i], ";", 1).at(0).c_str(), nullptr, 16);
		std::string	chunk_data	= splitted[i + 1];

		if (chunk_data.size() != (u_long)chunk_size)
			_requestStatus = REQ_INVALID; // error 400
		else
			_request["Body"] += chunk_data;
	}
	return false;
}

void	Request::parseHeader(std::string const &header)
{
	std::vector<std::string>	params	= split(header, "\r\n");
	std::vector<std::string>	tmp;

	tmp = split(params.at(0));
	_request.insert(std::pair<std::string, std::string>("Method", tmp.at(0)));
	_request.insert(std::pair<std::string, std::string>("Path", utils::replace(tmp.at(1), "%20", " ")));
	_request.insert(std::pair<std::string, std::string>("Protocol", tmp.at(2)));

	tmp = split(params.at(1), ":");
	_request.insert(std::pair<std::string, std::string>(tmp.at(0), trim(tmp.at(1)) + ":" + tmp.at(2))); // maybe remake

	for (u_int i = 2; i < params.size(); ++i)
	{
		tmp = split(params.at(i), ":");
		if (!tmp.at(0).empty() && tmp.size() == 2)
			_request.insert(std::pair<std::string, std::string>(tmp.at(0),trim(tmp.at(1))));
	}

	if (_request["Cookie"].empty())
		_request["Cookie"];
	if (_request["Content-Length"].empty())
		_request["Content-Length"];
	if (_request["Transfer-Encoding"].empty())
		_request["Transfer-Encoding"];
	_request["Host"] = replace(_request["Host"], "localhost", "127.0.0.1");
	std::vector<std::string>	tmp2 = split(_request["Path"], "?", 1);
	_request["Path"] = tmp2.at(0);
	if (_request["Body"].empty() && tmp2.size() == 2)
		_request["Body"] = tmp2.at(1);
//	if (!_request["Accept"].empty())
//	{
//		_request["Accept"] = split(_request["Accept"], ", ").at(0);
//		std::cout << _request["Accept"] << std::endl;
//	}
	_request["Accept"] = "*/*";

	if (!_transferEncoding)
	{
		if (!_request["Content-Length"].empty())
		{
			_contLength			= ::strtoul(_request["Content-Length"].c_str(), nullptr, 10);
			if (!_request["Content-Type"].empty() && _request["Content-Type"].find("multipart") != std::string::npos)
				_transferEncoding |= REQ_MULTI;
			else
				_transferEncoding |= REQ_SOLID;
		}
		else if (!_request["Transfer-Encoding"].empty() && (_request["Content-Length"].empty() ||
				 _request["Transfer-Encoding"].find("chunked") != std::string::npos))
			_transferEncoding	|= REQ_CHUNK;
		else
			_transferEncoding	|= REQ_OTHER;
	}

	_requestStatus |= REQ_HEAD;
	_head			= header;
}
