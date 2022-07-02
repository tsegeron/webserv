CC			=	c++
#FLAGS1		=	-Wall -Wextra -Werror
FLAGS2		=	-std=c++11

RM			=	rm -rf

# ------------------------------------------------------------------------------

FLDR_S	=	srcs/
FLDR_H	=	hdrs/

NAME		=	webserv
HDRS		=	SimpSocket.hpp		Utils.hpp			ConfigParams.hpp	\
				Config.hpp			Server.hpp			Request.hpp			\
				Response.hpp
SRCS		=	main.cpp			SimpSocket.cpp		Utils.cpp			\
				Config.cpp			Server.cpp			Request.cpp			\
				Response.cpp


SRC		=	$(addprefix ${FLDR_S},${SRCS})
HDR		=	$(addprefix ${FLDR_H},${HDRS})
OBJS	=	${SRC:%.cpp=%.o}

# ------------------------------------------------------------------------------

READY	=	Webserv is ready
SWEPT	=	Directory was cleaned
_GREEN	=	\e[32m
_YELLOW	=	\e[33m
_PURPLE	=	\e[0;35m
_CYAN	=	\e[0;36m
_END	=	\e[33m

# ------------------------------------------------------------------------------

%.o:		%.cpp	${HDR}
			@${CC} ${FLAGS1} ${FLAGS2} -c -o $@ $<

${NAME}:	${OBJS}
			@${CC} ${FLAGS1} ${FLAGS2} ${OBJS} -o ${NAME}
			@printf "${_CYAN}${READY}${_END}\n"
#			@${MAKE} clean

all:		${NAME}

clean:
			@${RM} ${OBJS}
			@printf "${_PURPLE}${SWEPT}${_END}\n"

fclean:		clean
			@${RM} ${NAME} webserv.logs

re:			fclean all

.PHONY:		re all clean fclean
