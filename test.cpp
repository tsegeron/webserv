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
	std::cout << utils::readFile("error_pages/404.html") << std::endl;
}