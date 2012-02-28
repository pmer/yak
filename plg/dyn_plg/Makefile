SO=dyn_plg.so
CFLAGS=-g -fPIC -I../..
LDFLAGS=-g -fPIC -shared
OBJS = 	dyn_plg.o

all: $(SO)

clean:
	$(RM) $(OBJS) $(SO)

$(SO): $(OBJS)
	$(CC) -o $(SO) $(OBJS) $(LDFLAGS)

