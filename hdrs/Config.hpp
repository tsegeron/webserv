//
// Created by Grubbly Ernesto on 6/8/22.
//

#ifndef ZERO_WEBSERV_CONFIG_HPP
# define ZERO_WEBSERV_CONFIG_HPP

# include "Utils.hpp"
# include "unistd.h"
# include <sys/stat.h>

# define CONF_DIR "config/"

class Config {
private:
	std::string	_configFilePathShort;
	std::string	_configFilePathFull;

	static bool	exists(std::string const &filename);
	static bool	is_config(std::string const &filename);
	std::string	readFile() const;

public:
	Config();
	Config(const char **av);
	Config(Config const &);
	Config & operator = (Config const &);
	virtual ~Config();

	bool	is_valid() const; // checks if such config file exists and if it has proper format

	void	parse();
};


#endif //ZERO_WEBSERV_CONFIG_HPP
