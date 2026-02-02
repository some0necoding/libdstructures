TEST_OBJ = dynarr.o hmap.o main.o
TEST_NAME = main

CFLAGS := -O2 -g -fPIC
LDFLAGS := -L../src -l:libds.a

all: $(TEST_NAME)

$(TEST_NAME): $(TEST_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY := clean
clean:
	rm -f *.o $(TEST_NAME)
