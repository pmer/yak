SO=hello.so
CFLAGS=-g -fPIC
LDFLAGS=-g -fPIC -shared
OBJS = \
	hello.o

all: $(SO)

clean:
	$(RM) $(OBJS) $(SO)

$(SO): $(OBJS)
	$(CC) -o $(SO) $(OBJS) $(LDFLAGS)
