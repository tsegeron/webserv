//
// Created by Grubbly Ernesto on 6/13/22.
//

#include "../hdrs/Response.hpp"

const std::map<int, std::string>	Response::_statusPhrase = Response::_createMap();

std::map<int, std::string> Response::_createMap()
{
	std::map<int, std::string>	m;

	m[200] = "OK";
	m[301] = "Moved Permanently";
	m[304] = "Not Modified";
	m[307] = "Temporary Redirect";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[500] = "Server Error";
	m[501] = "Not Implemented";
	m[503] = "Out of Resources";
	m[504] = "Gateway Time-Out";

	return m;
}

Response::Response(Params &config, Request *request)
	: _statusCode(200), _config(config), _request(request)
{
}

int		Response::process()
{
	std::string	param;

	_caseNum = 0;
	switch (action_to_do(param))
	{
		case bootstrap:
			_body = utils::readFile(utils::trim(_request->getPath(), "./"));
			break;
		case invalid:
			_body = utils::readFile(_config.root + _config.error_pages_dir + std::to_string(_statusCode) + ".html");
			if (_body.empty())
				_body = utils::readFile("error_pages/" + std::to_string(_statusCode) + ".html");
			break;
		case autoindexation:
			_response = getCgiResponse(_config.locations.at(param).at("cgi"),utils::trim(_request->getPath(), "/"));
			break;
		case cgi:
			_response = getCgiResponse(_config.locations.at(_request->getPath()).at("cgi"));
		case redirection:
		case other:
			break;
	}
	craftResponse();

	utils::logging(	_request->getMethod()				+ " " +
					_request->getPath()					+ " " +
					std::to_string(_statusCode)		+ " " +
					_statusPhrase.at(_statusCode)	+ " " +
					std::to_string(_body.size()));

	return _caseNum;
}

int			Response::action_to_do(std::string &param)
{
	if (_request->getPath().find("bootstrap") != std::string::npos)
		return _caseNum = bootstrap;
	if (is_redirect())
		return _caseNum = redirection;
	if (!(param = is_autoindex()).empty())
		return autoindexation;
	if (!is_valid())
		return _caseNum = invalid;
	else
		if (is_cgi())
			return cgi;
	return _caseNum = other;
}

std::string	Response::getCgiResponse(std::string const &path, std::string const &filename) const
{
	int		pid;
	int		fd[2];
	char	buf[10000];
	char	**tmp_env;

	bzero(&buf, 10000);
	tmp_env = getCgiEnv(path, filename);
	pipe(fd);
	pid = fork();
	if (!pid)
	{
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		close(fd[0]);
		if (execve(("./cgi-bin/" + path).c_str(), nullptr, tmp_env) <= 0)
			exit(3);
	}
	else
	{
		close(fd[1]);
		int status;
		waitpid(-1, &status, 0);
		read(fd[0], &buf, 10000);
		close(fd[0]);
		for (int i = 0; tmp_env[i]; ++i)
			free(tmp_env[i]);
		free(tmp_env);
	}
	return buf;
}

char	**Response::getCgiEnv(std::string const &path, std::string const &filename) const
{
	char						**env;
	std::vector<std::string>	envp_vec;
	std::vector<std::string>	tmp = utils::split(path, "?");
	std::string login("Vagiz");

//	envp_vec.push_back("PATH=" + std::string(std::getenv("PATH")));
	envp_vec.push_back("CONTENT_TYPE=" + _request->getAccept());
	envp_vec.push_back("CONTENT_LENGTH=" + std::to_string(1000));
	envp_vec.push_back("HTTP_USER_AGENT=" + _request->getRequest().at("User-Agent"));
	envp_vec.push_back("QUERY_STRING=" + _request->getBody());
//	envp_vec.push_back("REMOTE_ADDR=" + _request.get());
	envp_vec.push_back("REMOTE_HOST=" + _request->getHost());
	envp_vec.push_back("REQUEST_METHOD=" + _request->getMethod());
	envp_vec.push_back("SCRIPT_FILENAME=cgi-bin");
	envp_vec.push_back("SCRIPT_NAME=" + path);
	envp_vec.push_back("SERVER_NAME=" + _config.server_name);
	envp_vec.push_back("FILENAME=" + filename);
	envp_vec.push_back("COOKIE=" + _request->getCookie());
	envp_vec.push_back("LOGIN=" + login);
//	env = new char *[envp_vec.size()];
	env = (char **)::malloc(sizeof(char *) * (envp_vec.size() + 1));
	for (u_int i = 0; i < envp_vec.size(); i++)
		env[i] = strdup(envp_vec[i].c_str());
	env[envp_vec.size()] = nullptr;
//	for (const auto &elem : envp_vec)
//		std::cout << elem << std::endl;
//	exit(0);
	return env;
}


bool	Response::is_cgi()
{
	try {
		if (_config.locations[utils::split(_request->getPath(), "?").at(0)].empty())
			return false;
		if (_config.locations[utils::split(_request->getPath(), "?").at(0)]["cgi"].empty())
			return false;
		if (!_config.locations.at(utils::split(_request->getPath(), "?").at(0)).at("cgi").empty())
			return true;
		return false;
	}
	catch (...) { return false; }
}

bool	Response::is_redirect()
{
	try {
		if (_config.locations[_request->getPath()].empty())
			return false;
		if (!_config.locations.at(_request->getPath())["redirect"].empty())
			return _statusCode = 307;
		return false;
	}
	catch (...) { return false; }
}

std::string	Response::is_autoindex()
{
	try {
		std::vector<std::string>	tmp = utils::split(utils::trim(_request->getPath(), "/"), "/");
		std::string					str;

		for (u_int i = 0; i < tmp.size(); ++i)
		{
			str += "/" + tmp[i];
			if (_config.locations[str].empty())
				continue;
			if (_config.locations[str]["autoindex"] == "on")
				return str;
		}
		return "";
	}
	catch (...) { return ""; }
}

bool	Response::is_valid()
{
	std::map<std::string, std::string>	confLocation;

//	checks if requested PATH exists
	try {
		if (_config.locations[_request->getPath()].empty())
			return !(_statusCode = 404);
		confLocation = _config.locations.at(_request->getPath());
	}
	catch (...) { return !(_statusCode = 404); }

//	checks if requested METHOD allowed
	try {
		if (_request->getMethod() != "GET" && _request->getMethod() != "HEAD" &&
			_request->getMethod() != "POST" && _request->getMethod() != "PUT" &&
			_request->getMethod() != "DELETE")
			return !(_statusCode = 501);
//		if (_request->getMethod() == "HEAD" && confLocation.at("methods").find("GET") != std::string::npos)
//			return true;
		if (confLocation.at("methods").find(_request->getMethod()) == std::string::npos)
			return !(_statusCode = 405);
	}
	catch (...) { return !(_statusCode = 405); }

//	checks if requested ...
//	_request->getCookie()
	return true;
}

void	Response::craftResponse()
{
	if (_caseNum)
		craftHeader();
	else
	{
		_header	= split(_response, "\r\n\r\n", 1).at(0) + "\r\n\r\n";
		_body	= split(_response, "\r\n\r\n", 1).size() == 2 ? split(_response, "\r\n\r\n", 1).at(1) : "";
	}

	if (_request->getMethod() != "HEAD")
		_response = _header + _body;
	else
	{
		_response = _header;
		_body.clear();
	}
}

void	Response::craftHeader()
{
	_header = _request->getRequest()["Protocol"] + " " +
			  std::to_string(_statusCode) + " " +
			  _statusPhrase.at(_statusCode) + "\r\n" +
			  "Server: " + _config.server_name + "\r\n" +
			  "Connection: keep-alive\r\n" +
			  "Content-Length: " + std::to_string(_body.size()) + "\r\n";
	if (_statusCode == 301 || _statusCode == 307)
	{
		std::map<std::string, std::string>	tmp = _request->getRequest();
		_header += std::string("Location: ") + "http://" + tmp.at("Host") + "/" +
				utils::trim(_config.locations.at(_request->getPath()).at("redirect"), "/") + "\r";
//		std::cout << _header << std::endl;
	}
	if (_request->getPath().find("css") != std::string::npos)
		_header += "Content-Type: text/css\r\n\r\n";
	else
	_header += "Content-Type: " + _request->getAccept() + "\r\n\r\n";

}
