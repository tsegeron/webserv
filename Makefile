CC			=	c++
#FLAGS1		=	-Wall -Wextra -Werror
FLAGS2		=	-std=c++11

RM			=	rm -rf

# ------------------------------------------------------------------------------

FLDR_S	=	srcs/
FLDR_H	=	hdrs/

NAME		=	webserv
HDRS		=	IServer.hpp			Server.hpp			SimpSocket.hpp	\
				Utils.hpp			ConfigParams.hpp	Config.hpp		\
				Request.hpp			Response.hpp
SRCS		=	main.cpp			SimpSocket.cpp		Utils.cpp		\
				Config.cpp			Server.cpp			Request.cpp		\
				Response.cpp


SRC		=	$(addprefix ${FLDR_S},${SRCS})
HDR		=	$(addprefix ${FLDR_H},${HDRS})
OBJS	=	${SRC:%.cpp=%.o}

# ------------------------------------------------------------------------------

%.o:		%.cpp	${HDR}
			@${CC} ${FLAGS1} ${FLAGS2} -c -o $@ $<

${NAME}:	${OBJS}
			@${CC} ${FLAGS1} ${FLAGS2} ${OBJS} -o ${NAME}
#			@${MAKE} clean

all:		${NAME}

clean:
			@${RM} ${OBJS}

fclean:		clean
			@${RM} ${NAME} webserv.logs

re:			fclean all

.PHONY:		re all clean fclean
