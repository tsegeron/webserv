#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include "hdrs/Config.hpp"


std::string	readFile()
{
	std::ifstream	f("config/default.conf");
	std::string		data;
	std::string		tmp;

	while (std::getline(f, tmp))
		data += tmp + "\n";

	f.close();

	return data;
}

void	foo()
{
	std::vector<std::string>			pairParamsRoutes = utils::split(readFile(), "\n\n");
	std::vector<std::string>			tmp;
	std::vector<std::string>			tmp2;
	std::map<std::string, std::string>	params;

	tmp = utils::split(pairParamsRoutes.at(0), "\n");
	for (size_t i = 0; i < tmp.size(); ++i)
	{
		tmp2 = utils::split(tmp[i], ": ");
		params[tmp2.at(0)] = tmp2.at(1);
	}

	for (const auto &elem : params)
		std::cout << elem.first << " : " << elem.second << std::endl << "------------------------------------" << std::endl;
}



int main(int ac, char **av)
{
//	foo();
	Config	config(av);

	if (!config.is_valid())
		return 1;
	config.parse();

	for (const auto &elem : config.getParams())
		std::cout << elem.first << " : " << elem.second << std::endl;
//	std::cout	<< config.params.host << std::endl
//				<< config.params.port << std::endl
//				<< config.params.server_name << std::endl
//				<< config.params.error_pages_dir << std::endl
//				<< config.params.root << std::endl
//				<< config.params.default_error_page << std::endl
//				<< config.params.body_size_limit << std::endl;

}