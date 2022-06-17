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




int main(int ac, char **av, char **env)
{
	int len = -1;
	while(env[++len]);

	for (int i = 0; i < len; ++i)
		std::cout << env[i] << std::endl;
}