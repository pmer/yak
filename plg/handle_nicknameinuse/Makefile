SO=handle_nicknameinuse.so
CFLAGS=-g -fPIC -I../..
LDFLAGS=-g -fPIC -shared
OBJS = 	handle_nicknameinuse.o

all: $(SO)

clean:
	$(RM) $(OBJS) $(SO)

$(SO): $(OBJS)
	$(CC) -o $(SO) $(OBJS) $(LDFLAGS)

