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

	return *this;
}

Config::~Config()
{

}




void	Config::parse()
{
	std::vector<std::string>	pairParamsRoutes = utils::split(readFile(), "\n\n");

//	parseParams(pairParamsRoutes.at(0));
	parseRoutes(utils::trim(pairParamsRoutes.at(1), "\n"));

//	std::cout << utils::trim(pairParamsRoutes.at(1), "\n") << std::endl;

//	for (const auto &elem : pairParamsRoutes)
//		std::cout << elem << std::endl;
}

void	Config::parseParams(std::string const &src)
{
	std::vector<std::string>	paramsList = utils::split(src, "\n");
	std::vector<std::string>	paramsPair;

	for (size_t i = 0; i < paramsList.size(); i++)
	{
		paramsPair = utils::split(paramsList[i], ": ");
		if (paramsPair.size() > 1)
			_params[paramsPair.at(0)] = paramsPair.at(1);
	}
	params.host					= _params["Host"];
	params.port					= int(::strtol(_params["Port"].c_str(), nullptr, 10));
	params.server_name			= _params["Server_name"];
	params.error_pages_dir		= _params["Error_pages_dir"];
	params.body_size_limit		= ::strtol(_params["Body_size_limit"].c_str(), nullptr, 10);
	params.root					= _params["Root"];
	params.default_error_page	= _params["Default_error_page"];

	std::vector<std::string>	tmp(utils::split(_params["Error_pages"]));
	for (size_t i = 0; i < tmp.size(); i++)
	{
		paramsPair = utils::split(tmp[i], ".");
		if (paramsPair.size() > 1)
			params.error_pages[paramsPair.at(0)] = tmp[i];
	}
}

void	Config::parseRoutes(std::string const &src)
{
	std::vector<std::string>			routesList = utils::split(src, "location ");
	std::map<std::string, std::string>	map;

	for (size_t i = 0; i < routesList.size(); ++i)
	{
		map["uri"] = ;

		params.locations.insert(params.locations.begin(), map);
		map.clear();
	}

	for (const auto &elem : routesList)
		std::cout << elem;
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

// checks if such config file exists and if it has proper format
bool	Config::is_valid() const
{
	if (exists(_configFilePathFull) && is_config(_configFilePathFull))
	{
		utils::logging("applying config " + _configFilePathShort);
		return true;
	}
	utils::logging("Aborted. No config file was provided, neither default nor custom.", 2);
	return false;
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
