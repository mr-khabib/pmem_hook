TARGET := libhook.so
OBJS := libhook.o subop_check.o
CC := gcc
LDFLAGS := -ldl $(shell pkg-config --libs glib-2.0)
CFLAGS := $(CFLAGS) -Wall -fPIC -shared -D_GNU_SOURCE $(shell pkg-config --cflags glib-2.0)

TEST_TARGET := test
TEST_OBJS := main_test.o subop_check.o
TEST_LDFLAGS := $(shell pkg-config --libs glib-2.0)
TEST_CFLAGS := -Wall -g $(shell pkg-config --cflags glib-2.0)

$(TARGET): $(OBJS)
	    $(CC) $(CFLAGS) $^ -o$@ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	    $(CC) $(TEST_CFLAGS) $^ -o$@ $(TEST_LDFLAGS)
		echo "RUNNING TESTS"
		./test

clean:
		    rm -f $(TARGET) $(OBJS) $(TEST_TARGET) $(TEST_OBJS)

