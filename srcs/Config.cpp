//
// Created by Grubbly Ernesto on 6/8/22.
//

#include "../hdrs/Config.hpp"

Config::Config() // default config
{
	_configFilePathFull = "config/default.conf";
	_configFilePathShort = "default.conf";
}

Config::Config(const char **av)
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
			size_t	dot = filename.find_last_of(".");

			if (dot != std::string::npos) // is it .conf format?
				if (!filename.substr(0, dot).empty() && filename.substr(dot + 1) == "conf")
					return true;
		}
	}
	return false;
}
