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
	std::map<int, std::string> m = {
			{1, "11"},
			{2, "22"}
	};
	std::cout << m[1] << std::endl;
}