TARGET := libhook.so
OBJS := main.o subop_check.o
CC := gcc
LDFLAGS := -ldl 
CFLAGS := $(CFLAGS) -Wall -fPIC -shared -D_GNU_SOURCE

$(TARGET): $(OBJS)
	    $(CC) $(CFLAGS) $^ -o$@ $(LDFLAGS)

clean:
	    rm -f $(TARGET) $(OBJS)
#gcc main.c -o libhook.so -fPIC -shared -ldl -D_GNU_SOURCE

