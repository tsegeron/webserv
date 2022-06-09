//
// Created by Grubbly Ernesto on 6/8/22.
//

#ifndef ZERO_WEBSERV_CONFIG_HPP
# define ZERO_WEBSERV_CONFIG_HPP

# include "Utils.hpp"
# include "unistd.h"
# include <sys/stat.h>
# include <map>

# define CONF_DIR "config/"

struct Params {
	typedef std::map<std::string, std::string>				strPairMap;
	typedef std::vector<std::map<std::string, std::string>> ElonMask;
	std::string	host;
	std::string	server_name;
	std::string	error_pages_dir;
	std::string	root;
	std::string	default_error_page;
	strPairMap	error_pages;
	int			port;
	size_t		body_size_limit;
	ElonMask	locations;
};

class Config {
public:
	typedef std::map<std::string, std::string>	strPairMap;
	Params	params;

private:
	std::string	_configFilePathShort;
	std::string	_configFilePathFull;
	strPairMap	_params;

	static bool	exists(std::string const &filename);
	static bool	is_config(std::string const &filename);
	std::string	readFile() const;
	void		parseParams(std::string const &);
	void		parseRoutes(std::string const &);

public:
	Config(char **av);
	Config(Config const &);
	Config & operator = (Config const &);
	virtual ~Config();


	bool	is_valid() const; // checks if such config file exists and if it has proper format
	void	parse();

	strPairMap	getParams() const { return _params; };
};


#endif //ZERO_WEBSERV_CONFIG_HPP
