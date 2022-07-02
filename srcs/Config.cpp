//
// Created by Grubbly Ernesto on 6/8/22.
//

#include "../hdrs/Config.hpp"

Config::Config(char **av)
{
	std::ofstream	configFile;
	std::string		rawPath;
	int				i = -1;

	_configFilePathShort = "default.conf";
	while(av[++i]);
	if (i != 2)
		_configFilePathFull = CONF_DIR + std::string("default.conf");
	else
	{
		rawPath = av[1];
		if (!exists(rawPath))
		{
			if (exists(CONF_DIR + rawPath))
			{
				_configFilePathFull = CONF_DIR + rawPath;
				_configFilePathShort = rawPath;
			}
			else
				_configFilePathFull = "config/default.conf";
		}
		else
		{
			_configFilePathFull		= rawPath;
			_configFilePathShort	= rawPath.find_last_of('/') != std::string::npos ?
					rawPath.substr(rawPath.find_last_of('/') + 1) : rawPath;
		}
	}
}

Config::Config(Config const &other)
{
	*this = other;
}

Config & Config::operator = (Config const &other)
{
	this->_configFilePathFull = other._configFilePathFull;
	this->_configFilePathShort = other._configFilePathShort;
	this->_error_msg = other._error_msg;
	this->_params = other._params;

	return *this;
}

Config::~Config()
{
	for (u_int i = 0; i < _params.size(); ++i)
		_params[i].clear();
	_params.clear();
}


bool	Config::is_valid()
{
	if (exists(_configFilePathFull) && is_config(_configFilePathFull))
	{
		parse();
		if (!is_params_valid())
		{
			utils::logging(_error_msg, 2);
			return false;
		}
		utils::logging("Applying config " + _configFilePathShort, 3);
		return true;
	}
	utils::logging("Aborted. No config file was provided, neither default nor custom.", 2);
	return false;
}

bool	Config::is_params_valid()
{
	std::map<std::string, std::vector<int> >	portsDuplicateCheck;

	if (!_error_msg.empty())
		return false;
	if (_params.empty())
	{
		_error_msg = "Insufficient configuration";
		return false;
	}
	for (u_int i = 0; i < _hostsPorts.size(); ++i)
	{
		if (std::find(portsDuplicateCheck[_hostsPorts[i].first].begin(),
					  portsDuplicateCheck[_hostsPorts[i].first].end(),
					  _hostsPorts[i].second) != portsDuplicateCheck[_hostsPorts[i].first].end())
		{
			_error_msg = "[Config] Ports duplication on the same host was found. --> " +
					_hostsPorts[i].first + ":" + std::to_string(_hostsPorts[i].second);
			return false;
		}
		portsDuplicateCheck[_hostsPorts[i].first].push_back(_hostsPorts[i].second);
	}
	portsDuplicateCheck.clear();
	return true;
}

void	Config::parse()
{
	std::vector<std::string>	pairParamsRoutes = utils::split(readConfig(), "\n\n");

	if (pairParamsRoutes.empty() || pairParamsRoutes.size() % 2)
		return;
	for (size_t i = 0; i < pairParamsRoutes.size(); i += 2)
	{
		parseParams(pairParamsRoutes.at(i));
		if (!_error_msg.empty())
			break;
		parseRoutes(utils::trim(pairParamsRoutes.at(i + 1), "\n"));
		if (!_error_msg.empty())
			break;
		if (!_tmp.empty())
			_params.push_back(_tmp);
		_tmp.clear();
	}
	for (size_t i = 0; i < _params.size(); ++i)
	{
		if (i == 0)
		{
			if (_params[i].host.empty())
				_params[i].host = "127.0.0.1";
			if (_params[i].port == 0)
				_params[i].port = 8080;
			if (_params[i].server_name.empty())
				_params[i].server_name = "webserv";
			if (_params[i].error_pages_dir.empty())
				_params[i].error_pages_dir = "/error_pages";
			if (_params[i].body_size_limit == 0)
				_params[i].body_size_limit = 60000;
			if (_params[i].root.empty())
				_params[i].root = "./";
//			if (_params[i].error_pages.empty())
//				_params[i].error_pages["404"] = "404.html";
		}
		if (_params[i].port == 0 || _params[i].server_name.empty() ||
			_params[i].root.empty() || _params[i].locations.empty())
		{
			_error_msg = "Make sure all the parameters are set";
			return ;
		}
		_hostsPorts.push_back(std::pair<std::string, int>(_params[i].host, _params[i].port));
	}
}

void	Config::parseParams(std::string const &src)
{
	std::vector<std::string>	paramsList = utils::split(src, "\n");
	std::vector<std::string>	paramsPair;
	strPairMap					tmpParams;

	for (size_t i = 0; i < paramsList.size(); ++i)
	{
		paramsPair = utils::split(paramsList[i], ": ");
		if (paramsPair.empty() || paramsPair.size() != 2)
		{
			_error_msg = "Wrong config file structure";
			return;
		}
		tmpParams[paramsPair.at(0)] = paramsPair.at(1);
	}
	_tmp.host					= tmpParams["Host"];
	_tmp.port					= int(::strtol(tmpParams["Port"].c_str(), nullptr, 10));
	_tmp.server_name			= tmpParams["Server_name"];
	_tmp.error_pages_dir		= tmpParams["Error_pages_dir"];
	_tmp.body_size_limit		= ::strtol(tmpParams["Body_size_limit"].c_str(), nullptr, 10);
	_tmp.root					= tmpParams["Root"];

	std::vector<std::string>	tmp(utils::split(tmpParams["Error_pages"]));
	for (size_t i = 0; i < tmp.size(); ++i)
	{
		paramsPair = utils::split(tmp[i], ".");
		if (paramsPair.size() > 1)
			_tmp.error_pages[paramsPair.at(0)] = tmp[i];
	}
}

void	Config::parseRoutes(std::string const &src)
{
	std::vector<std::string>			routesList = utils::split(src, "location ");
	std::vector<std::string>			tmp;
	std::vector<std::string>			valPair;
	std::map<std::string, std::string>	map;
	std::string							location;

	if (routesList.empty())
	{
		_error_msg = "Wrong config file structure";
		return;
	}

	for (size_t i = 0; i < routesList.size(); ++i)
	{
		tmp = utils::split(routesList.at(i), " {");
		if (tmp.size() != 2)
		{
			_error_msg = "Config error: expected '{' after location";
			return;
		}
		location = tmp.at(0);
		tmp = utils::split(tmp.at(1), "\n");
		if (std::find(tmp.begin(), tmp.end(), "}") == tmp.end())
		{
			_error_msg = "Config error: expected closing curly braces '}' after location parameters";
			return;
		}
		tmp.erase(std::find(tmp.begin(), tmp.end(), "}"));
		for (size_t j = 0; j < tmp.size(); ++j)
		{
			valPair = utils::split(utils::trim(tmp.at(j)), ": ");
			if (valPair.size() != 2)
			{
				_error_msg = "[Config] Parse error: error near " + valPair.at(0);
				return;
			}
			map[valPair.at(0)] = valPair.at(1);
		}
		if (!is_location_valid(map))
		{
			_error_msg = "Config error: error near " + _tmp.host + ":" + std::to_string(_tmp.port) + " location '" + location +
					"'. Location is expected to have 'page', 'cgi', 'redirect', 'autoindex' or 'root' parameter";
			return;
		}
		_tmp.locations[location] = map;
		map.clear();
	}
//	for (auto &elem : params.locations)
//	{
//		for (auto &kv : elem)
//			std::cout << kv.first << " : " << kv.second << std::endl;
//		std::cout << std::endl << std::endl;
//	}
}

std::string	Config::readConfig() const
{
	std::ifstream	f(_configFilePathFull);
	std::string		data;
	std::string		tmp;

	while (std::getline(f, tmp))
		data += tmp + '\n';

	f.close();

	return utils::trim(data, "\n");
}

bool	Config::exists(std::string const &filename)
{
	return ::access(filename.c_str(), F_OK) == 0;
}

bool	Config::is_config(std::string const &filename)
{
	struct stat	s;

	if (::lstat(filename.c_str(), &s) == 0 && !filename.empty()) // is it valid path?
	{
		if (S_ISREG(s.st_mode) && ::access(filename.c_str(), X_OK) != 0) // is it text file, not executable?
		{
			size_t	dot = filename.find_last_of('.');

			if (dot != std::string::npos) // is it .conf format?
				if (!filename.substr(0, dot).empty() && filename.substr(dot + 1) == "conf")
					return true;
		}
	}
	return false;
}

bool	Config::is_location_valid(std::map<std::string, std::string> uri_params)
{
	if (uri_params["page"].empty() && uri_params["cgi"].empty() &&
		uri_params["redirect"].empty() && uri_params["autoindex"].empty() &&
		uri_params["root"].empty())
		return false;
	return true;
}
