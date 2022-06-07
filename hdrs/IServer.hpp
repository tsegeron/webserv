//
// Created by Grubbly Ernesto on 6/7/22.
//

#ifndef ZERO_WEBSERV_ISERVER_HPP
#define ZERO_WEBSERV_ISERVER_HPP

# include "SimpSocket.hpp"

class IServer {
private:
	virtual bool		accepter(struct sockaddr_in, int) = 0;
	virtual void		handler(long) = 0;
	virtual void		responder() = 0;

public:
	virtual void		runServer() = 0;

	virtual SimpSocket	*getServSocket() const = 0;

};


#endif //ZERO_WEBSERV_ISERVER_HPP
