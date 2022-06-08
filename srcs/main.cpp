#include "../hdrs/Server.hpp"

int main(const int ac, const char **av)
{
	Config	config(av);

	utils::createLogFile();
	if (!config.is_valid())
		return 1;
	try
	{
		Server testServer(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY, 1000);

		testServer.runServer();
	}
	catch (std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
	}

	return 0;
}
