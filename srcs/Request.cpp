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
	_request.clear();
}

void	Request::parseRequest()
{
	std::vector<std::string>	request	= utils::split(_rawRequest, "\r\n\r\n");
	std::vector<std::string>	params	= utils::split(request.at(0), "\r\n");
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

	_request["Accept"] = utils::trim(utils::split(_request.at("Accept"), ",").at(0));
	if (request.size() == 1)
		_request["Body"] = std::string();
	else
		_request["Body"] = utils::trim(request.at(1));
}

//int main()
//{
//	std::string	src = "GET / HTTP/1.1\r\n"
//						 "Host: localhost:8080\r\n"
//						 "Connection: keep-alive\r\n"
//						 "Cache-Control: max-age=0\r\n"
//						 "sec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"101\", \"Google Chrome\";v=\"101\"\r\n"
//						 "sec-ch-ua-mobile: ?0\r\n"
//						 "sec-ch-ua-platform: \"macOS\"\r\n"
//						 "Upgrade-Insecure-Requests: 1\r\n"
//						 "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.64 Safari/537.36\r\n"
//						 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
//						 "Sec-Fetch-Site: none\r\n"
//						 "Sec-Fetch-Mode: navigate\r\n"
//						 "Sec-Fetch-User: ?1\r\n"
//						 "Sec-Fetch-Dest: document\r\n"
//						 "Accept-Encoding: gzip, deflate, br\r\n"
//						 "Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7\r\n"
//						 "Cookie: Clion-c50c84ee=b54ba6ba-8bb0-48d1-9b04-5205e1da3e13\r\n"
//						 "\r\n"
//						 "\r\n"
//						 "\r\n"
//						 "";
////	int i = 0;
////	std::vector<std::string>	vec = utils::split(src, "\n");
////	for (auto &elem : vec)
////		std::cout << ++i << ":\t"<< elem << std::endl;
//	Request	req(src);
//
//	req.parseRequest();
////	std::cout << req.getMethod() << std::endl;
////	std::cout << req.getPath() << std::endl;
////	std::cout << req.getCookie() << std::endl;
//
//	for (const auto& [key, value] : req.getRequest())
//		std::cout << '[' << key << "] = " << value << "; " << std::endl;
//
//}
