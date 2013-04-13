all: zklua.so

# NOTE: you should modify the $(ZOOKEEPER_C_API_DIR) variable to
# the name of your zookeeper c api diretory that are copied from
# apache zookeeper manually(This can help you to avoid installing
# the exstra zookeeper c libraries into your system if you do not
# want them to be installed), and usually the zookeeper c api resides
# in the following diretory: `zookeeper-X.Y.Z/src/c`, where X.Y.Z is
# the zookeeper version. You may change the name `zookeeper-X.Y.Z/src/c`
# to `zookeeper-c-api-X.Y.Z` according to your zookeeper version when
# copying that diretory to the current zklua diretory.
ZOOKEEPER_C_API_DIR = ./zookeeper-c-api-3.4.5
ZOOKEEPER_INCLUDE_DIR = $(ZOOKEEPER_C_API_DIR)/include/
ZOOKEEPER_MT_AR_LIB = $(ZOOKEEPER_C_API_DIR)/.libs/libzookeeper_mt.a

CC = gcc
CFLAGS = `pkg-config lua5.1 --cflags` -I$(ZOOKEEPER_INCLUDE_DIR) -fPIC -O2 #-Wall
INSTALL_PATH = `pkg-config lua5.1 --variable=INSTALL_CMOD`

OS_NAME = $(shell uname -s)
MH_NAME = $(shell uname -m)

LDFLAGS += -lm -ldl -lpthread

ifeq ($(OS_NAME), Darwin)
LDFLAGS += -bundle -undefined dynamic_lookup -framework CoreServices
ifeq ($(MH_NAME), x86_64)
endif
else
LDFLAGS += -shared -lrt
endif

$(ZOOKEEPER_MT_AR_LIB): $(ZOOKEEPER_C_API_DIR)/Makefile
	$(MAKE) -C $(ZOOKEEPER_C_API_DIR)

$(ZOOKEEPER_C_API_DIR)/Makefile:
	cd $(ZOOKEEPER_C_API_DIR) && ./configure
	cd $(ZOOKEEPER_C_API_DIR) && cp ./generated/zookeeper.jute.h ./include/zookeeper.jute.h

SRCS := zklua.c

OBJS := $(patsubst %.c,%.o,$(SRCS))

LIBS = $(ZOOKEEPER_MT_AR_LIB)

$(OBJS):
	$(CC) -c $(CFLAGS) $(SRCS)

zklua.so: $(LIBS) $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)


.PHONY: all clean distclean install

clean:
	rm -f *.o *.so

distclean:clean
	$(MAKE) -C $(ZOOKEEPER_C_API_DIR) clean
	$(MAKE) -C $(ZOOKEEPER_C_API_DIR) distclean
	cd $(ZOOKEEPER_C_API_DIR) && rm ./include/zookeeper.jute.h

install: zklua.so
	install -D -s zklua.so $(INSTALL_PATH)/zklua.so
