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
//	std::string tmp = utils::readFile("../bootstrap5/js/bootstrap.js/bootstrap.min.js");
	std::string tmp = utils::readFile("error_pages/501.html");
	std::cout << tmp.c_str() << std::endl;
}