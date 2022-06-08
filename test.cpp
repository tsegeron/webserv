#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>

bool	exists(std::string const &filename)
{
	return ::access(filename.c_str(), F_OK) == 0;
}

bool	is_config(std::string const &filename)
{
	struct stat	s;

	if (::lstat(filename.c_str(), &s) == 0 && !filename.empty()) // is it valid path?
	{
		if (S_ISREG(s.st_mode) && ::access(filename.c_str(), X_OK) != 0) // is it text file, not executable?
		{
			size_t	dot = filename.find_last_of(".");

			if (dot != std::string::npos) // is it .conf format?
				if (!filename.substr(0, dot).empty() && filename.substr(dot + 1) == "conf")
					return true;
		}
	}
	return false;
}

int main(int ac, char **av)
{
//	std::cout << is_config("config/default.conf") << std::endl;
	std::cout << is_config("d.conf") << std::endl;
//	std::cout << exists("webserv.logs") << std::endl;
}