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
GLIB = pkg-config --cflags --libs glib-2.0

all: server

#
# You should be able to add object files here without changing anything else
#
OBJ_FILES = project1.cc

server: ${OBJ_FILES}
	${LD} ${LDFLAGS} ${OBJ_FILES} -o $@ `${GLIB}`

#
# Please remember not to submit objects or binarys.
#
clean:
	rm -f server

#
# This might work to create the submission tarball in the formal I asked for.
#
submit:
	rm -f server
	mkdir `whoami`
	cp makefile README.txt *.h *.cc `whoami`
	tar cf `whoami`.tar `whoami`
