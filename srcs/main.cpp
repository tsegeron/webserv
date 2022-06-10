#include "../hdrs/Server.hpp"

int main(int ac, char **av)
{
//	Config	config(av);
//
//	utils::createLogFile();
//	if (!config.is_valid())
//		return 1;
	try
	{
		Server testServer(av);

		testServer.runServer();
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
