TARGET := libhook.so
OBJS := main.o
CC := gcc
LDFLAGS := -ldl 
CFLAGS := $(CFLAGS) -fPIC -shared -D_GNU_SOURCE

$(TARGET): $(OBJS)
	    $(CC) $(CFLAGS) $^ -o$@ $(LDFLAGS)

clean:
	    rm -f $(TARGET) $(OBJS)
#gcc main.c -o libhook.so -fPIC -shared -ldl -D_GNU_SOURCE
