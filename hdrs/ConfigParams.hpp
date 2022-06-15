//
// Created by Grubbly Ernesto on 6/10/22.
//

#ifndef ZERO_WEBSERV_CONFIGPARAMS_HPP
# define ZERO_WEBSERV_CONFIGPARAMS_HPP

# include "Utils.hpp"
# include <map>

struct Params {
	typedef std::map<std::string, std::string>							strPairMap;
	typedef std::map<std::string, std::map<std::string, std::string> >	ElonMask;
	std::string	host;
	std::string	server_name;
	std::string	error_pages_dir;
	std::string	root;
	std::string	default_error_page;
	strPairMap	error_pages;
	int			port;
	size_t		body_size_limit;
	ElonMask	locations;

	Params & operator = (Params const &other) {
		host = other.host;
		server_name = other.server_name;
		error_pages_dir = other.error_pages_dir;
		root = other.root;
		default_error_page = other.default_error_page;
		error_pages = other.error_pages;
		port = other.port;
		body_size_limit = other.body_size_limit;
		locations = other.locations;

		return *this;
	}
	void	clear() {
		host = "";
		server_name = "";
		error_pages_dir = "";
		root = "";
		default_error_page = "";
		error_pages.clear();
		port = 0;
		body_size_limit = 0;
		locations.clear();
	}
	bool	empty()	{
		return host.empty() && server_name.empty() && error_pages_dir.empty() &&
			   root.empty() && default_error_page.empty() && error_pages.empty() &&
			   port == 0 && body_size_limit == 0 && locations.empty();
	}
};


#endif //ZERO_WEBSERV_CONFIGPARAMS_HPP
