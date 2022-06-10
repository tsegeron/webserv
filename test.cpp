#include <iostream>
#include <string>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include <fstream>
#include "hdrs/Config.hpp"

int main(int ac, char **av)
{
//	foo();
//	Config	config(av);
//
//	if (!config.is_valid())
//		return 1;
//	config.parse();
//
//	for (const auto &elem : config.getParams())
//		std::cout << elem.first << " : " << elem.second << std::endl;
//	std::cout	<< config.params.host << std::endl
//				<< config.params.port << std::endl
//				<< config.params.server_name << std::endl
//				<< config.params.error_pages_dir << std::endl
//				<< config.params.root << std::endl
//				<< config.params.default_error_page << std::endl
//				<< config.params.body_size_limit << std::endl;

	std::string	test("");
	std::string	test1("");
	std::string	test2("a");

	std::cout << (test.empty() && test1.empty() && test2.empty()) << std::endl;
//	for (auto &elem : utils::split(test, "\n"))
//		std::cout << elem.empty() << std::endl;



}