//
// Created by Grubbly Ernesto on 6/7/22.
//

#ifndef ZERO_WEBSERV_ISERVER_HPP
#define ZERO_WEBSERV_ISERVER_HPP

# include "SimpSocket.hpp"

class IServer {
private:
	virtual void		accepter(struct sockaddr_in &, int &) = 0;
	virtual void		handler(int) = 0;
	virtual void		responder(int) = 0;

public:
	virtual void		runServer() = 0;

};


#endif //ZERO_WEBSERV_ISERVER_HPP
