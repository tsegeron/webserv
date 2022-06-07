CC			=	c++
#FLAGS1		=	-Wall -Wextra -Werror
FLAGS2		=	-std=c++11

RM			=	rm -rf

# ------------------------------------------------------------------------------

FLDR_S	=	srcs/
FLDR_H	=	hdrs/

NAME		=	webserv
HDRS		=	IServer.hpp	Server.hpp	SimpSocket.hpp	Utils.hpp
SRCS		=	main.cpp	Server.cpp	SimpSocket.cpp	Utils.cpp

SRC		=	$(addprefix ${FLDR_S},${SRCS})
HDR		=	$(addprefix ${FLDR_H},${HDRS})
OBJS	=	${SRC:%.cpp=%.o}

# ------------------------------------------------------------------------------

%.o:		%.cpp	${HDR}
			@${CC} ${FLAGS1} ${FLAGS2} -c -o $@ $<

${NAME}:	${OBJS}
			@${CC} ${FLAGS1} ${FLAGS2} ${OBJS} -o ${NAME}

all:		${NAME}

clean:
			@${RM} ${OBJS}

fclean:		clean
			@${RM} ${NAME}

re:			fclean all

.PHONY:		re all clean fclean
