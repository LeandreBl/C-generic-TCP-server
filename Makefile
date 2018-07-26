##
## EPITECH PROJECT, 2017
##
## File description:
## makefile
##

NAME		= liblblserver.so

CC		= gcc

LIBS		=

SRCS		= src/lserver.c
SRCS		+= src/update.c
SRCS		+= src/add_del_listener.c
SRCS		+= src/client.c
SRCS		+= src/eject.c
SRCS		+= src/link.c
SRCS		+= src/set_backlog.c

TESTS_SRCS := $(SRCS)
TESTS_SRCS += tests/cbuffer_tests.c

OBJS		= $(SRCS:.c=.o)
TESTS_OBJS = $(TESTS_SRCS:.c=.o)

RM		= rm -f

CFLAGS		= -Werror -Wall -Wextra -fPIC -pedantic
CFLAGS		+= -I ./include
LDFLAGS		= -shared

GREEN		= '\033[0;32m'
NO_COLOR	= '\033[0m'

%.o : %.c
	@$ $(CC) $(CFLAGS) -c $< -o $@
	@echo "$(CC) $(CFLAGS) -c $< -o $@ ["$(GREEN)"OK"$(NO_COLOR)"]"
.SUFFIXES: .o .c

all: $(NAME)

$(NAME): $(OBJS)
	@$ $(CC) $(LDFLAGS) $(OBJS) -o $@
	@echo "$(CC) $(LDFLAGS) $(OBJS) -o $@ \
	["$(GREEN)"LINKING OK"$(NO_COLOR)"]"

tests_run: $(TESTS_OBJS)
	@$ $(CC) -lcriterion $(TESTS_OBJS) -o $@
	@echo "$(CC) -lcriterion $(TESTS_OBJS) -o $@ \
	["$(GREEN)"LINKING OK"$(NO_COLOR)"]"
	./$@
	@$(RM) $@
	@$(RM) $(TESTS_OBJS)

debug: CFLAGS += -g3
debug: re

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

install: re
	@cp $(NAME) /usr/lib/$(NAME) 2> /dev/null || \
	printf "\033[1m\033[31mError : try sudo make install\033[0m\n" && \
	cp include/*.h /usr/include/ 2> /dev/null && \
	printf "\033[1m\033[32mLibrary successfull installed !\033[0m\n"

.PHONY: all clean fclean re tests_run debug install
