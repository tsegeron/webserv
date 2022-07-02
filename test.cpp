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
//#include "hdrs/Utils.hpp"
//#include "hdrs/Config.hpp"

void foo()
{
	static int a;

	std::cout << ++a << std::endl;
}

int main(int ac, char **av, char **env)
{
	for (int i = 0; i < 10; ++i)
		foo();
}