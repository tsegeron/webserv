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

	std::string	&trim(std::string &src, std::string const &chars)
	{
		src.erase(src.find_last_not_of(chars) + 1);
		src.erase(0, src.find_first_not_of(chars));

		return src;
	}

	std::vector<std::string>	split(std::string src, std::string const &delimiter)
	{
		std::vector<std::string>	splitted;
		std::string					tmp;
		size_t						pos;

		while ((pos = src.find(delimiter)) != std::string::npos)
		{
			tmp = src.substr(0, pos);
			if (!tmp.empty())
				splitted.push_back(tmp);
			src.erase(0, pos + 1);
		}
		if (!src.empty())
			splitted.push_back(src);

		return splitted;
	}

}

//int main()
//{
//	std::string	source = "Hello world she said to me eventually";
//	std::vector<std::string>	splitted = utils::split(source);
//	for (std::string const &i : splitted)
//		std::cout << i << std::endl;
//}
