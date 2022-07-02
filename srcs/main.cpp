#include "../hdrs/Server.hpp"

int main(int ac, char **av)
{
	(void) ac;
	try
	{
		Server testServer(av);
		testServer.runServer();
	}
	catch (std::exception &ex)
	{
		std::cerr << RED << ex.what() << RESET << std::endl;
	}

	return 0;
}
