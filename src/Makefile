LIB_OBJ = dynarr.o hmap.o types.o hash.o
LIB_NAME = libds.a

CFLAGS := -O2 -g -fPIC

all: $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJ)
	$(AR) rcs $@ $^

.PHONY := clean
clean:
	rm -f *.o $(LIB_NAME)
