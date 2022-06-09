#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include "hdrs/Utils.hpp"


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
	std::string							data = readFile();
	std::vector<std::string>			pairParamsRoutes = utils::split(data, "\n\n");
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
	foo();
}