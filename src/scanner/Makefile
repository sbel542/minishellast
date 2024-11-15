# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: abelov <abelov@student.42london.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/15 18:49:40 by abelov            #+#    #+#              #
#    Updated: 2024/11/15 18:49:40 by abelov           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

PROGRAM			:= scanner

OBJS = lex.yy.o
SRCS = lex.yy.c

CC				:= cc
INC_DIR			=  ./include
INCLUDE_FLAGS	:= -I. -I$(INC_DIR) -I/usr/include
OPTIMIZE_FLAGS	:= -O0
DEBUG_FLAGS		:= -g3 -gdwarf-3
MANDATORY_FLAGS	:= -Wall -Wextra -Werror -Wimplicit
CFLAGS			= $(MANDATORY_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZE_FLAGS) $(INCLUDE_FLAGS)
LINK_FLAGS		:= -lfl


all: $(PROGRAM)

.c.o: $(SRCS)
	$(CC) $(CFLAGS) -c $*.c -o $@ -O

lex.yy.c: scanner.l
	flex -d $^

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LINK_FLAGS)