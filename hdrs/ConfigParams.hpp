//
// Created by Grubbly Ernesto on 6/10/22.
//

#ifndef ZERO_WEBSERV_CONFIGPARAMS_HPP
# define ZERO_WEBSERV_CONFIGPARAMS_HPP

# include "Utils.hpp"

struct Params {
	typedef std::map<std::string, std::string>							strPairMap;
	typedef std::map<std::string, std::map<std::string, std::string> >	strMapPairMap;
	std::string		host;
	std::string		server_name;
	std::string		error_pages_dir;
	std::string		root;
	std::string		cgi;
	strPairMap		error_pages;
	int				port;
	strMapPairMap	locations;

	Params & operator = (Params const &other) {
		host			= other.host;
		server_name		= other.server_name;
		error_pages_dir	= other.error_pages_dir;
		root			= other.root;
		error_pages		= other.error_pages;
		port			= other.port;
		locations		= other.locations;

		return *this;
	}
	void	clear() {
		host			= "";
		server_name		= "";
		error_pages_dir	= "";
		root			= "";
		port			= 0;
		error_pages.clear();
		locations.clear();
	}
	bool	empty()	{
		return host.empty() && server_name.empty() && error_pages_dir.empty() &&
			   root.empty() && error_pages.empty() && port == 0 && locations.empty();
	}
};

#endif //ZERO_WEBSERV_CONFIGPARAMS_HPP
