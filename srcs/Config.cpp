//
// Created by Grubbly Ernesto on 6/8/22.
//

#include "../hdrs/Config.hpp"

typedef std::vector<std::map<std::string, std::string>> ElonMask;

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
		utils::logging("applying config " + _configFilePathShort);
		return true;
	}
	utils::logging("Aborted. No config file was provided, neither default nor custom.", 2);
	return false;
}

bool	Config::is_params_valid()
{
	if (_params.empty())
	{
		_error_msg = "Insufficient configuration";
		return false;
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
				_params[i].root = "root/";
			if (_params[i].default_error_page.empty())
				_params[i].root = "404.html";
			if (_params[i].error_pages.empty())
				_params[i].root = "404.html";
		}
		if (_params[i].port == 0 || _params[i].server_name.empty() ||
			_params[i].root.empty() || _params[i].locations.empty())
		{
			_error_msg = "Make sure all the parameters are set";
			return false;
		}
	}
	return true;
}

void	Config::parse()
{
	std::vector<std::string>	pairParamsRoutes = utils::split(readFile(), "\n\n");

	if (pairParamsRoutes.empty() || pairParamsRoutes.size() % 2)
		return;
	for (size_t i = 0; i < pairParamsRoutes.size(); i += 2)
	{
		parseParams(pairParamsRoutes.at(i));
		parseRoutes(utils::trim(pairParamsRoutes.at(i + 1), "\n"));
		if (!_tmp.empty())
			_params.push_back(_tmp);
		_tmp.clear();
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
		if (paramsPair.empty())
		{
			_error_msg = "Wrong config file structure";
			return;
		}
		if (paramsPair.size() > 1)
			tmpParams[paramsPair.at(0)] = paramsPair.at(1);
	}
	_tmp.host					= tmpParams["Host"];
	if (tmpParams["Port"].empty())
		tmpParams["Port"] = "0";
	_tmp.port					= int(::strtol(tmpParams["Port"].c_str(), nullptr, 10));
	_tmp.server_name			= tmpParams["Server_name"];
	_tmp.error_pages_dir		= tmpParams["Error_pages_dir"];
	if (tmpParams["Body_size_limit"].empty())
		tmpParams["Body_size_limit"] = "60000";
	_tmp.body_size_limit		= ::strtol(tmpParams["Body_size_limit"].c_str(), nullptr, 10);
	_tmp.root					= tmpParams["Root"];
	_tmp.default_error_page	= tmpParams["Default_error_page"];

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

	if (routesList.empty())
	{
		_error_msg = "Wrong config file structure";
		return;
	}

	for (size_t i = 0; i < routesList.size(); ++i)
	{
		tmp = utils::split(routesList.at(i), " {");
		map["uri"] = tmp.at(0);
		tmp = utils::split(tmp.at(1), "\n");
		tmp.erase(std::find(tmp.begin(), tmp.end(), "}"));
		for (size_t j = 0; j < tmp.size(); ++j)
		{
			valPair = utils::split(utils::trim(tmp.at(j)), ": ");
			if (!valPair.at(0).empty())
				map[valPair.at(0)] = valPair.at(1);
		}
		_tmp.locations.push_back(map);
		map.clear();
	}
//	for (auto &elem : params.locations)
//	{
//		for (auto &kv : elem)
//			std::cout << kv.first << " : " << kv.second << std::endl;
//		std::cout << std::endl << std::endl;
//	}
}

std::string	Config::readFile() const
{
	std::ifstream	f("config/default.conf");
	std::string		data;
	std::string		tmp;

	while (std::getline(f, tmp))
		data += tmp + '\n';

	f.close();

	return data;
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
