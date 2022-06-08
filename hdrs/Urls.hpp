//
// Created by Grubbly Ernesto on 6/8/22.
//

#ifndef ZERO_WEBSERV_URLS_HPP
# define ZERO_WEBSERV_URLS_HPP

# include "Utils.hpp"
# include "Config.hpp"
# include "Request.hpp"

class Urls {
private:
	Config	config;

	Urls(Urls const &other);
	Urls & operator = (Urls const &other);

public:
	Urls();
	virtual ~Urls();

	void	views(Request const &request);
};


#endif //ZERO_WEBSERV_URLS_HPP
