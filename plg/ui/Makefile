SO=ui.so
CFLAGS=-g -fPIC -I../.. -pthread
LDFLAGS=-g -fPIC -shared -lreadline -pthread
OBJS = 	ui.o

all: $(SO)

clean:
	$(RM) $(OBJS) $(SO)

$(SO): $(OBJS)
	$(CC) -o $(SO) $(OBJS) $(LDFLAGS)

