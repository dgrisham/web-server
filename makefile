#
# Makefile
# Project 4
#
#  Created by Phillip Romig on 4/3/12.
#  Copyright 2012 Colorado School of Mines. All rights reserved.
#

CC = gcc
LD = gcc
CFLAGS = -g -pthread -Wall
LDFLAGS = -g -pthread -Wall
CLIFLAGS = -g -Wall
GLIB = pkg-config --cflags --libs glib-2.0
SERVDIR = ./src/server
CLIDIR = ./src/client

all: server client

#
# You should be able to add object files here without changing anything else
#
SERV_OBJ_FILES = ${SERVDIR}/server.c
CLI_OBJ_FILES = ${CLIDIR}/client.c

server: ${SERVER_OBJ_FILES}
	${LD} ${LDFLAGS} ${SERV_OBJ_FILES} -o $@ `${GLIB}`

client: ${CLI_OBJ_FILES}
	${CC} ${CLIFLAGS} ${CLI_OBJ_FILES} -o $@

#
# Please remember not to submit objects or binarys.
#
clean:
	rm -f server client

#
# This might work to create the submission tarball in the formal I asked for.
#
submit:
	rm -f server client
	mkdir `whoami`
	cp makefile README.txt *.h *.c `whoami`
	tar cf `whoami`.tar `whoami`
