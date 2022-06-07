//
// Created by Grubbly Ernesto on 6/7/22.
//

#ifndef ZERO_WEBSERV_UTILS_HPP
# define ZERO_WEBSERV_UTILS_HPP

# include <string>
# include <iostream>
# include <fstream>

namespace utils {

	std::ofstream	openLogFile();
	void			logging(std::string const &, int fd = 1);
	void			closeLogFile();
}

#endif //ZERO_WEBSERV_UTILS_HPP
