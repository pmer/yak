OPT=-g
CFLAGS = -std=gnu99 $(OPT) $(BLDCFLAGS)
LDFLAGS = -lpcre -ldl -rdynamic $(OPT) $(BLDCFLAGS)

OBJECTS = \
	acl.o \
	callback.o \
	chan.o \
	diagnostic.o \
	hashtab.o \
	ircproto.o \
	plg.o \
	presence.o \
	privmsg.o \
	regex.o \
	socket.o \
	usr.o \
	yak.o \


all: yak

yak: $(OBJECTS)

clean:
	$(RM) $(OBJECTS) yak

