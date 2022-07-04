#include "../hdrs/Utils.hpp"
#include "../hdrs/Request.hpp"

namespace utils {

	void	createLogFile()
	{
		std::ofstream	logFile("webserv.logs", std::ios::trunc);
	}

	void	logging(std::string const &msg, int fd)
	{
		std::ofstream	logFile("webserv.logs", std::ios::app);
		time_t      	t = time(nullptr);
		struct tm   	*tm;
		char        	timestamp[23];

		timestamp[22] = '\0';
		tm = ::gmtime(&t);
		tm->tm_hour += 3;
		::strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]\t", tm);

		logFile << timestamp + msg << std::endl;
		logFile.close();

		switch (fd) {
			case serverInfo:
				std::cout << BLUE << timestamp << RESET << GREEN << msg << RESET << std::endl;
				break;
			case error:
				std::cerr << BLUE << timestamp << RESET << RED << msg << RESET << std::endl;
				break;
			case connectionInfo:
				std::cout << BLUE << timestamp << RESET << CYAN << msg << RESET << std::endl;
				break;
			default:
				std::cout << msg << std::endl;
		}
	}

	std::string	ltrim(std::string src, std::string const &chars)
	{
		src.erase(0, src.find_first_not_of(chars));

		return src;
	}

	std::string	rtrim(std::string src, std::string const &chars)
	{
		src.erase(src.find_last_not_of(chars) + 1);

		return src;
	}

	std::string	trim(std::string src, std::string const &chars)
	{
		src.erase(src.find_last_not_of(chars) + 1);
		src.erase(0, src.find_first_not_of(chars));

		return src;
	}

	std::vector<std::string>	split(std::string src, std::string const &delimiter, int maxsplit)
	{
		std::vector<std::string>	splitted;
		std::string					tmp;
		size_t						pos;

		while (!src.empty() && maxsplit)
		{
			pos = src.find(delimiter);
			if (pos == 0)
			{
				src.erase(0, delimiter.size());
				continue;
			}
			tmp = src.substr(0, pos);

			src.erase(0, pos);
			if (!tmp.empty() || (tmp.empty() && src.empty()))
			{
				splitted.push_back(tmp);
				maxsplit--;
			}
		}
		while (src.find(delimiter) == 0)
			src.erase(0, delimiter.size());
		if (splitted.empty() || !src.empty())
			splitted.push_back(src);

		return splitted;
	}

	std::vector<std::string>	split_chunk(std::string src)
	{
		std::vector<std::string>	splitted;
		std::string					tmp;
		size_t						pos;

		do {
			pos = src.find("\r\n");
			if (pos == std::string::npos)
				return splitted;
			splitted.push_back(src.substr(0, pos));
			src.erase(0, pos + 2);
		} while(!src.empty());

		return splitted;
	}


	std::string	readFile(std::string const &filename)
	{
		std::ifstream	f(filename);
		std::string		data;
		std::string		tmp;

		if (f.is_open())
			while (std::getline(f, tmp))
				data += utils::trim(tmp);
		f.close();

		return utils::trim(data);
	}

	void	print_rawRequest(std::string const &request)
	{
		std::cout << MAGENTA << "---------Reading request---------" << RESET << std::endl;
		std::cout << request << std::endl;
		std::cout << MAGENTA << "---------------------------------" << RESET << std::endl;
	}

	void	print_fullRequest(std::map<std::string, std::string> const &request)
	{
		std::cout << MAGENTA << "---------Reading request---------" << RESET << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = request.cbegin(); it != request.end(); ++it)
			std::cout << it->first << " : " << it->second << std::endl;
		std::cout << MAGENTA << "---------------------------------" << RESET << std::endl;
	}

	void	print_shortRequest(Request const *request)
	{
		std::cout << MAGENTA << "---------Reading request---------" << RESET << std::endl;
		std::cout	<< request->getMethod() << " "
					 << request->getPath() << " "
					 << request->getAccept() << std::endl
					 << "Body: " << request->getBody() << std::endl;
//				<< request->getCookie() << std::endl;
		std::cout << MAGENTA << "---------------------------------" << RESET << std::endl;
	}

	std::string	to_lower(std::string src)
	{
		for (u_long i = 0; i < src.size(); ++i)
			src[i] = (char)tolower(src[i]);
		return src;
	}
}
/*
std::vector<std::string>	split(std::string src, std::string const &delimiter, int maxsplit)
{
	std::vector<std::string>	splitted;
	std::string					tmp;
	size_t						pos;

	src = ltrim(src, delimiter);
	while ((pos = src.find(delimiter)) != std::string::npos && maxsplit)
	{
		tmp = src.substr(0, pos);
		if (!tmp.empty())
		{
			if (!splitted.empty() && (--splitted.end())->empty())
			{
				splitted.pop_back();
				maxsplit++;
			}
			splitted.push_back(tmp);
			maxsplit--;
		}
		else if (!splitted.empty() && !((--splitted.end())->empty()))
		{
			splitted.push_back(tmp);
			maxsplit--;
		}
		src.erase(0, pos + delimiter.size());
	}
	if (!src.empty() || splitted.empty())
	{
		if (!splitted.empty() && (--splitted.end())->empty())
			splitted.pop_back();
		splitted.push_back(trim(src, delimiter));
	}

	return splitted;
}
 */