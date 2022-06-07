//
// Created by Grubbly Ernesto on 6/7/22.
//

#include "../hdrs/Request.hpp"

Request & Request::operator = (Request const &src)
{
	this->_rawRequest	= src._rawRequest;
	this->_request		= src._request;
	return *this;
}

Request::~Request()
{

}

void	Request::parseRequest()
{
	std::vector<std::string>	params = utils::split(_rawRequest, "\n");
	std::vector<std::string>	tmp;

	tmp = utils::split(params.at(0));
	_request.insert(std::pair<std::string, std::string>("Method", tmp.at(0)));
	_request.insert(std::pair<std::string, std::string>("Path", tmp.at(1)));
	_request.insert(std::pair<std::string, std::string>("Protocol", tmp.at(2)));

	tmp = utils::split(params.at(1), ":");
	_request.insert(std::pair<std::string, std::string>(tmp.at(0), utils::trim(tmp.at(1)) + ":" + tmp.at(2)));

	for (int i = 2; i < params.size(); ++i)
	{
		tmp = utils::split(params.at(i), ":");
		if (!tmp.at(0).empty())
			_request.insert(std::pair<std::string, std::string>(tmp.at(0),utils::trim(tmp.at(1))));
	}
}

//int main()
//{
//	std::string	src = "GET / HTTP/1.1\n"
//						 "Host: localhost:8080\n"
//						 "Connection: keep-alive\n"
//						 "Cache-Control: max-age=0\n"
//						 "sec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"101\", \"Google Chrome\";v=\"101\"\n"
//						 "sec-ch-ua-mobile: ?0\n"
//						 "sec-ch-ua-platform: \"macOS\"\n"
//						 "Upgrade-Insecure-Requests: 1\n"
//						 "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.64 Safari/537.36\n"
//						 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\n"
//						 "Sec-Fetch-Site: none\n"
//						 "Sec-Fetch-Mode: navigate\n"
//						 "Sec-Fetch-User: ?1\n"
//						 "Sec-Fetch-Dest: document\n"
//						 "Accept-Encoding: gzip, deflate, br\n"
//						 "Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7\n"
//						 "Cookie: Clion-c50c84ee=b54ba6ba-8bb0-48d1-9b04-5205e1da3e13\n"
//						 "\n"
//						 "\n"
//						 "\n"
//						 "";
////	int i = 0;
////	std::vector<std::string>	vec = utils::split(src, "\n");
////	for (auto &elem : vec)
////		std::cout << ++i << ":\t"<< elem << std::endl;
//	Request	req(src);
//
//	req.parseRequest();
//	std::cout << req.getMethod() << std::endl;
//	std::cout << req.getPath() << std::endl;
//	std::cout << req.getCookie() << std::endl;
//
////	for (const auto& [key, value] : dict)
////		std::cout << '[' << key << "] = " << value << "; " << std::endl;
//
//}
