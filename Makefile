all: zklua.so

# You may need to change the following variables according to
# your system, here are the descriptions of these variables:
#
# ZOOKEEPER_LIB_DIR: Zookeeper C API library path.
# LUA_LIB_DIR: Lua library path.
# LUA_VERSION: Lua version.
# LUA_VERSION_NUMBER: Lua version number.
ZOOKEEPER_LIB_DIR = /usr/local/lib
LUA_LIB_DIR = /usr/local/lib/lua
LUA_VERSION = lua
LUA_VERSION_NUMBER = 5.1

CC = gcc
CFLAGS = `pkg-config --cflags $(LUA_VERSION)` -fPIC -O2 #-Wall
# INSTALL_PATH = `pkg-config $(LUA_VERSION) --variable=INSTALL_CMOD`
INSTALL_PATH = $(LUA_LIB_DIR)/$(LUA_VERSION_NUMBER)

OS_NAME = $(shell uname -s)
MH_NAME = $(shell uname -m)

ifeq ($(OS_NAME), Darwin)
LDFLAGS += -bundle -undefined dynamic_lookup -framework CoreServices
ifeq ($(MH_NAME), x86_64)
endif
else
LDFLAGS += -shared -lrt
endif

LDFLAGS += -lm -ldl -lpthread -L$(ZOOKEEPER_LIB_DIR) -lzookeeper_mt

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
