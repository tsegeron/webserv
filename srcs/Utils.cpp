#include "../hdrs/Utils.hpp"

namespace utils {
	std::ofstream	openLogFile()
	{
		std::ofstream	logFile;

		if (!logFile.is_open())
			logFile.open("webserv.logs", std::ios::trunc);

		return logFile;
	}

	void	logging(std::string const &msg, int fd)
	{
		time_t      t = time(nullptr);
		struct tm   *tm;
		char        timestamp[23];

		timestamp[22] = '\0';
		tm = gmtime(&t);
		tm->tm_hour += 3;
		strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]\t", tm);

		openLogFile() << timestamp + msg << std::endl;

		if (fd == 1)
			std::cout << BLUE << timestamp << RESET << GREEN << msg << RESET << std::endl;
		else
			std::cerr << BLUE << timestamp << RESET << RED << msg << RESET << std::endl;

	}

	void	closeLogFile()
	{
		openLogFile().close();
	}

}
