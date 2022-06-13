#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <map>
//#include "hdrs/Config.hpp"
//#include "hdrs/SimpSocket.hpp"
#include "hdrs/Utils.hpp"


int main(int ac, char **av)
{
//	Config	config(av);
//
//	std::cout << config.getHostsPortsPair()[0].first << std::endl;

	std::map<int, std::string>	tmp;

	tmp[1] = "11";
	tmp[2] = "22";
	tmp[3] = "33";
	std::cout << (tmp.find(4) == tmp.end()) << std::endl;
//	test.push_back(std::pair<std::string, int>("1", 1));
//
//	std::cout << test[0].first << std::endl;


}