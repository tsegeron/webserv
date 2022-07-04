#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <map>
#include <set>
//#include "hdrs/Config.hpp"
//#include "hdrs/SimpSocket.hpp"
#include "hdrs/Utils.hpp"
//#include "hdrs/Config.hpp"


int main(int ac, char **av, char **env)
{
	std::string					str		= "124111some data";
//	std::string					str		= "\r\n";
	std::vector<std::string>	words	= split(str, "111", 0);
//
	std::cout << words.size() << std::endl << "---------------------" << std::endl;

	for (const auto &elem : words)
		std::cout << elem << std::endl;


//	std::cout << ::strtol("1a\r\n", nullptr, 16) << std::endl;
}