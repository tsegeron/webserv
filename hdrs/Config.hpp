//
// Created by Grubbly Ernesto on 6/8/22.
//

#ifndef ZERO_WEBSERV_CONFIG_HPP
# define ZERO_WEBSERV_CONFIG_HPP

# include "Utils.hpp"
# include "ConfigParams.hpp"
# include <unistd.h>
# include <sys/stat.h>
# include <map>

# define CONF_DIR "config/"

class Config {
	typedef typename std::map<std::string, std::string>			strPairMap;
	typedef typename std::vector<std::pair<std::string, int>>	HostsPortsPair;

private:
	std::string			_configFilePathShort;
	std::string			_configFilePathFull;
	std::string			_error_msg;
	std::vector<Params>	_params; // ConfigParams.hpp
	HostsPortsPair		_hostsPorts;
	Params				_tmp;

	static bool	exists(std::string const &filename);
	static bool	is_config(std::string const &filename);
	std::string	readConfig() const;
//	void
	void		parseParams(std::string const &);
	void		parseRoutes(std::string const &);
	bool		is_params_valid();
	void		parse();

public:
	Config(char **av);
	Config(Config const &);
	Config & operator = (Config const &);
	virtual ~Config();


	bool	is_valid();

	std::vector<Params>	getParams() const { return _params; };
	std::string			getErrorMsg() const { return _error_msg; };
	HostsPortsPair		getHostsPortsPair() const {  return _hostsPorts; };
};


#endif //ZERO_WEBSERV_CONFIG_HPP
