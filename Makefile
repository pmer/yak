OPT = -g
CFLAGS := -std=gnu99 -pthread $(OPT) $(CFLAGS)
LDFLAGS := -lpcre -pthread -ldl -rdynamic $(OPT) $(LDFLAGS)

OBJECTS = \
	auth.o \
	callback.o \
	chan.o \
	diagnostic.o \
	hashtab.o \
	ircproto.o \
	plg.o \
	pref.o \
	presence.o \
	privmsg.o \
	regex.o \
	socket.o \
	str.o \
	thread.o \
	usr.o \
	yak.o \


all: yak

yak: $(OBJECTS)

clean:
	$(RM) $(OBJECTS) yak

