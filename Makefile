all: zklua.so

ZOOKEEPER_LIB_DIR = /usr/local/lib

CC = gcc
CFLAGS = `pkg-config lua5.1 --cflags` -I$(ZOOKEEPER_INCLUDE_DIR) -fPIC -O2 #-Wall
INSTALL_PATH = `pkg-config lua5.1 --variable=INSTALL_CMOD`

OS_NAME = $(shell uname -s)
MH_NAME = $(shell uname -m)

LDFLAGS += -lm -ldl -lpthread -L$(ZOOKEEPER_LIB_DIR) -lzookeeper_mt

ifeq ($(OS_NAME), Darwin)
LDFLAGS += -bundle -undefined dynamic_lookup -framework CoreServices
ifeq ($(MH_NAME), x86_64)
endif
else
LDFLAGS += -shared -lrt
endif

SRCS := zklua.c

OBJS := $(patsubst %.c,%.o,$(SRCS))

$(OBJS):
	$(CC) -c $(CFLAGS) $(SRCS)

zklua.so: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


.PHONY: all clean install

clean:
	rm -f *.o *.so

install: zklua.so
	install -D -s zklua.so $(INSTALL_PATH)/zklua.so
