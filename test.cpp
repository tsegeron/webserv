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
	std::string					str		= "12;\r\nSome Data\r\n";
	std::vector<std::string>	words	= split(str, "\r\n", 2);

	std::cout << words.size() << std::endl << "---------------------" << std::endl;

	for (const auto &elem : words)
		std::cout << elem;


//	std::cout << ::strtol("01a\r\n", nullptr, 16) << std::endl;
}