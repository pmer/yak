SO=eval.so
CFLAGS=-g -fPIC -I../..
LDFLAGS=-g -fPIC -shared
OBJS = 	eval.o

all: $(SO)

clean:
	$(RM) $(OBJS) $(SO)

$(SO): $(OBJS)
	$(CC) -o $(SO) $(OBJS) $(LDFLAGS)
