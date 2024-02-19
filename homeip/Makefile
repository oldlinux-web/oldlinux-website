#
# Home IP detect Makefile
#

CC      = gcc
CFLAGS  = -g 
LFLAGS  = -L. -liniparser
AR      = ar
ARFLAGS = rcv
RM      = rm -f


default: all

all: homeipd2  # homeipc

homeipd2: homeipd2.c
	$(CC) $(CFLAGS) -o homeipd2 homeipd2.c -L. -liniparser

#parse: parse.c
#        $(CC) $(CFLAGS) -o parse parse.c -L. -liniparser

clean veryclean:
	$(RM) homeipd2 homeipc
