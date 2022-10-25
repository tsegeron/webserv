//
// Created by Grubbly Ernesto on 6/13/22.
//

#include "../hdrs/Response.hpp"
const std::map<int, std::string>	Response::_statusPhrase		= Response::_createStatusPhrasesMap();
const std::string					Response::_imageMIMETypes	= ".bmp.cod.gif.ief.jpe.jpeg.jpg.svg.tif.tiff"
																	".ras.cmx.ico.pnm.pbm.pgm.ppm.rgb.xbm.xpm.xwd.png";

std::map<int, std::string> Response::_createStatusPhrasesMap()
{
	std::map<int, std::string>	m;

	m[200] = "OK";
	m[201] = "Created";
	m[301] = "Moved Permanently";
	m[304] = "Not Modified";
	m[307] = "Temporary Redirect";
	m[400] = "Bad Request";
	m[401] = "Unauthorized";
	m[403] = "Forbidden";
	m[404] = "Not Found";
	m[405] = "Method Not Allowed";
	m[413] = "Payload Too Large";
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
			_body = readFile(trim(_request->getPath(), "./"));
			break;
//		case cgi_com:
//			getCgiResponse(_config.root + split(_config.cgi, " ").at(1));
//			break;
        case image:
            if (getImageBytes())
                break;
			_statusCode = 404;
		case autoindexation:
            if (_statusCode == 200)
                getCgiResponse(_config.locations.at(param).at("cgi"), trim(_request->getPath(), "/"));
            if (_statusCode == 200)
                break;
		case cgi_loc:
            if (_statusCode == 200)
                getCgiResponse(_config.locations.at(_request->getPath()).at("cgi"));
            if (_statusCode != 200)
                _caseNum = invalid;
            else
                break;
		case page:
			if (!param.empty() && _statusCode == 200)
			{
				if (!(_body = readFile(param)).empty())
					break;
				else
					_statusCode = 500;
				_caseNum = invalid;
			}
		case file_modif:
			if (_statusCode == 200 && handleModification(param))
				break;
		case invalid:
			_body = readFile(_config.root + _config.error_pages_dir + std::to_string(_statusCode) + ".html");
			if (_body.empty())
				_body = readFile("error_pages/" + std::to_string(_statusCode) + ".html");
			break;
		case redirection:
			break;
	}
	craftResponse();

//	logging(_request->getMethod()				+ " " +
//			_request->getPath()					+ " " +
//			std::to_string(_statusCode)		+ " " +
//			_statusPhrase.at(_statusCode)	+ " " +
//			std::to_string(_body.size()));

	return _caseNum;
}

bool	Response::handleModification(std::string const &filename)
{

	if (_request->getMethod() == "PUT" || _request->getMethod() == "POST")
	{
		if (Config::exists(filename))
			_statusCode = 200;
		else
			_statusCode = 201;

		std::ofstream	fout(filename, std::ofstream::out | std::ofstream::trunc);

		if (!fout.is_open())
		{
			_statusCode = 500;
			return false;
		}
		fout.write(_request->getBody().c_str(), (long)_request->getBody().size());
		fout.close();
	}
	else
	{
		if (Config::exists(filename) && Config::is_file(filename))
			std::remove(filename.c_str());
	}
	return true;
}

int		Response::action_to_do(std::string &param)
{
	std::vector<std::string>	tmp		= split(trim(_request->getPath(), "/"), "/");
	std::string					attempt_loc, location;

	if (!is_body_slim())
		return _caseNum = invalid;
	if (_request->getPath().find("bootstrap") != std::string::npos)
		return _caseNum = bootstrap;

	for (u_int i = 0; i < tmp.size(); ++i)
	{
		attempt_loc += "/" + tmp[i];
		if (_config.locations[attempt_loc].empty())
		{
			_config.locations.erase(attempt_loc);
			continue;
		}
		else
			location = attempt_loc != "/" ? rtrim(attempt_loc, "/") : attempt_loc;
	}
	if (location.empty())
	{
		_statusCode = 404;
		return  _caseNum = invalid; // if location Not Found
	}

	if (!is_method_alowed(location))
		return _caseNum = invalid;
	if (!(param = is_file_modification(location)).empty())
		return _caseNum = file_modif;
	if (is_image())
		return _caseNum = image;

	if (is_redirect(location))
		return _caseNum = redirection;
	if (is_autoindex(location))
	{
		param = location;
		return autoindexation;
	}
	if (!(param = is_page(location)).empty())
		return _caseNum = page;
	if (_request->getMethod() == "GET" && location != _request->getPath())
	{
		_statusCode = 404;
		return  _caseNum = invalid; // if location Not Found
	}
	if (is_cgi(location))
		return cgi_loc;
	return _caseNum = invalid;
}

std::string	Response::is_file_modification(std::string const &location)
{
	if (_request->getMethod() == "PUT" || _request->getMethod() == "DELETE" ||
	(_request->getMethod() == "POST" && location != _request->getPath()))
	{
		std::string	storage = _config.locations[location]["storage"];
		if (storage.empty())
		{
			_config.locations[location].erase("storage");
			_statusCode = 500;
			return "";
		}
		std::string	root	= trim(_config.locations[location]["root"], "/");
		if (!root.empty())
			root += "/";
		std::string	path	= trim(replace(_request->getPath(), location, root + storage), "/");

		if (Config::exists(path.substr(0, path.find_last_of('/'))) && Config::is_directory(path.substr(0, path.find_last_of('/'))))
			return path;
		else
		{
			_statusCode = 403;
			return "";
		}
	}
	return "";
}

std::string Response::is_page(std::string const &location)
{
	std::string	root	= trim(_config.locations[location]["root"], "/");
	std::string	path	= trim(replace(_request->getPath(), location, root), "/");
	std::string	pageFile;

	if (!_config.locations[location]["cgi"].empty())
		return "";
	if (Config::exists(path))
	{
		if (Config::is_directory(path))
		{
			if (!_config.locations[location]["page"].empty())
				pageFile = path + "/" + _config.locations[location]["page"];
			else
			{
				_config.locations[location].erase("page");
				pageFile =  path + "/index.html";
			}
		}
		else if (Config::is_file(path))
			pageFile = path;
		else
			_statusCode = 500; // no access to the given path (no rights)
	}
	else
		_statusCode = 404; // not found

	if (!pageFile.empty() && Config::exists(pageFile) && Config::is_file(pageFile))
		return pageFile;
	else
		_statusCode = 404; // not found

	return "";
}

bool	Response::getImageBytes() {
	std::ifstream fin(trim(_request->getPath(), "/"));

	if (!fin.is_open())
		return false;

	char	c;

	while (!fin.eof())
	{
		fin.read(&c, 1);
		_body += c;
	}
	fin.close();

	return true;
}

void    Response::getCgiResponse(std::string const &path, std::string const &filename)
{
    int		pid;
    int		fd[2];
    int		status;
    char	buf[10000];
    char	**tmp_env;
    std::string buf_ret;

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
            exit(255);
    }
    else {
        close(fd[1]);
        while (read(fd[0], &buf, 10000) > 0) {
            _response += buf;
            bzero(&buf, 10000);
        }
        waitpid(pid, &status, 0);
        close(fd[0]);
        for (int i = 0; tmp_env[i]; ++i)
            free(tmp_env[i]);
        free(tmp_env);
    }
    if (WEXITSTATUS(status) != 0)
        _statusCode = 500;
}


char	**Response::getCgiEnv(std::string const &path, std::string const &filename) const
{
	char						**env;
	std::vector<std::string>	envp_vec;
	std::vector<std::string>	tmp = split(path, "?");

	envp_vec.push_back("CONTENT_TYPE=" + _request->getAccept());
	envp_vec.push_back("HTTP_USER_AGENT=" + _request->getRequest().at("User-Agent"));
    envp_vec.push_back("QUERY_STRING=" + _request->getBody());
	envp_vec.push_back("REMOTE_HOST=" + _request->getHost());
	envp_vec.push_back("REMOTE_PORT=" + _request->getPort());
	envp_vec.push_back("REQUEST_METHOD=" + _request->getMethod());
	envp_vec.push_back("SERVER_NAME=" + _config.server_name);
	envp_vec.push_back("FILENAME=" + filename);
	envp_vec.push_back("COOKIE=" + _request->getCookie());
	env = (char **)::malloc(sizeof(char *) * (envp_vec.size() + 1));
	for (u_int i = 0; i < envp_vec.size(); i++)
		env[i] = strdup(envp_vec[i].c_str());
	env[envp_vec.size()] = nullptr;
	return env;
}

bool	Response::is_body_slim()
{
	if (_config.locations[_request->getPath()].empty() || _config.locations[_request->getPath()]["maxBody"].empty())
		return true;

	long	maxBody;
	if ((maxBody = ::strtol(_config.locations[_request->getPath()]["maxBody"].c_str(), nullptr, 10)) == 0)
		return true;

	if (_request->getRawRequest().size() <= (u_long)maxBody)
		return true;

	_statusCode = 413;

	std::vector<std::string>	tmp	= split(split(_request->getBody(), "&").at(0), "img_path=");
	if (tmp.size() == 1 && (Config::exists(tmp.at(0)) && Config::is_file(tmp.at(0))))
		std::remove(tmp.at(0).c_str());

	return false;
}

bool	Response::is_image()
{
	std::string	reqstr = _request->getPath();

	if (reqstr.find_last_of('.') != std::string::npos && reqstr.substr(reqstr.find_last_of('.')).size() >= 4)
		return _imageMIMETypes.find(reqstr.substr(reqstr.find_last_of('.'))) != std::string::npos;

	return false;
}

bool	Response::is_cgi(std::string const &location)
{
	try {
		if (_config.locations[location]["cgi"].empty())
		{
			_config.locations[location].erase("cgi");
			return false;
		}
		if (!_config.locations.at(location).at("cgi").empty())
			return true;
		return false;
	}
	catch (...) { return false; }
}

bool	Response::is_redirect(std::string const &location)
{
	try {
//		if (_config.locations[_request->getPath()].empty())
//			return false;
		if (!_config.locations.at(location)["redirect"].empty())
			return _statusCode = 307;
		_config.locations.at(location).erase("redirect");
		return false;
	}
	catch (...) { return false; }
}

bool	Response::is_autoindex(std::string const &location)
{
//	try {
//		std::vector<std::string>	tmp = split(trim(_request->getPath(), "/"), "/");
//		std::string					str;
//
//		for (u_int i = 0; i < tmp.size(); ++i)
//		{
//			str += "/" + tmp[i];
//			if (_config.locations[str].empty())
//				continue;
//			if (_config.locations[str]["autoindex"] == "on")
//				return str;
//		}
//		return "";
//	}
//	catch (...) { return ""; }

//	if (_config.locations[location].empty())
//		continue;
	if (_config.locations[location]["autoindex"] == "on")
		return true;
	else
		_config.locations[location].erase("autoindex");
	return false;
}

bool	Response::is_method_alowed(std::string const &location)
{
	std::map<std::string, std::string>	confLocation = _config.locations.at(location);

	if (is_image())
		return true;

//	checks if requested METHOD allowed
	try {
		if (_request->getMethod() != "GET" && _request->getMethod() != "HEAD" &&
			_request->getMethod() != "POST" && _request->getMethod() != "PUT" &&
			_request->getMethod() != "DELETE")
			return !(_statusCode = 501);
		if (confLocation.at("methods").find(_request->getMethod()) == std::string::npos)
			return !(_statusCode = 405);
	}
	catch (...) { return !(_statusCode = 405); }

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
			  "Connection: close\r\n" +
			  "Content-Length: " + std::to_string(_body.size()) + "\r\n";
	if (_statusCode == 301 || _statusCode == 307)
	{
		std::map<std::string, std::string>	tmp = _request->getRequest();
		_header += std::string("Location: ") + "http://" + tmp.at("Host") + "/" +
				trim(_config.locations.at(_request->getPath()).at("redirect"), "/") + "\r";
//		std::cout << _header << std::endl;
	}
	if (_request->getPath().find("css") != std::string::npos)
		_header += "Content-Type: text/css\r\n\r\n";
	else
		_header += "Content-Type: " + _request->getAccept() + "\r\n\r\n";
}

/*
std::map<std::string, std::string>	Response::_createImageMIMETypesMap()
{
	std::map<std::string, std::string>	m;

	m[".bmp"]	= "image/bmp";
	m[".cod"]	= "image/cis-cod";
	m[".gif"]	= "image/gif";
	m[".ief"]	= "image/ief";
	m[".jpe"]	= "image/jpeg";
	m[".jpeg"]	= "image/jpeg";
	m[".jpg"]	= "image/jpeg";
	m[".jfif"]	= "image/pipeg";
	m[".svg"]	= "image/svg+xml";
	m[".tif"]	= "image/tiff";
	m[".tiff"]	= "image/tiff";
	m[".ras"]	= "image/x-cmu-raster";
	m[".cmx"]	= "image/x-cmx";
	m[".ico"]	= "image/x-icon";
	m[".pnm"]	= "image/x-portable-anymap";
	m[".pbm"]	= "image/x-portable-bitmap";
	m[".pgm"]	= "image/x-portable-graymap";
	m[".ppm"]	= "image/x-portable-pixmap";
	m[".rgb"]	= "image/x-rgb";
	m[".xbm"]	= "image/x-xbitmap";
	m[".xpm"]	= "image/x-xpixmap";
	m[".xwd"]	= "image/x-xwindowdump";

	return m;
}
 */
