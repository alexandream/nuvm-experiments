CC=clang

CFLAGS=-pedantic -Wall -std=c89 -g
LIBS=

SOURCE_FILES=$(wildcard src/*.c src/objects/*.c)
#OBJ_FILES=$(SOURCE_FILES:src/%.c=build/nuvm/%.o)

TESTED_OBJ_FILES=$(SOURCE_FILES:src/%.c=build/tested/%.o)
TESTED_CFLAGS=$(CFLAGS) -DRUNNING_TESTS -Werror

TEST_CFLAGS=$(CFLAGS) -I src/ -Wno-variadic-macros -Werror
TEST_LIBS=-L build/tested -lnuvm -L build/atest -latest

TEST_SOURCE_FILES=$(wildcard tests/suites/*.c)
TEST_OBJ_FILES=$(TEST_SOURCE_FILES:tests/suites/%.c=build/tests/%.o)

all:build/tested/libnuvm.a

#-include $(patsubst %.o, %.d, $(OBJ_FILES))
-include $(patsubst %.o, %.d, $(TESTED_OBJ_FILES))


build/tested/libnuvm.a: $(TESTED_OBJ_FILES)
	@ar rcs build/tested/libnuvm.a $^


build/atest/libatest.a: tests/atest/src/atest.c tests/atest/src/atest.h
	@${CC} -o build/atest.o -c tests/atest/src/atest.c ${TEST_CFLAGS}
	@ar rcs build/atest/libatest.a build/atest.o


build/tested/%.o: src/%.c
	@${CC} -c $< -o $@ ${TESTED_CFLAGS}
	@${CC} -MM -MT $@ $(TESTED_CFLAGS) $< > $(@:.o=.d)

build/tests/runner: build/tested/libnuvm.a \
                   build/atest/libatest.a \
                   build/tests/runner.o \
                   $(TEST_OBJ_FILES)
	@${CC} -o build/tests/runner $^ ${TEST_LIBS}

build/tests/runner.o: tests/runner.c
	@${CC} -c $< -o $@ ${TEST_CFLAGS}


build/tests/%.o: tests/suites/%.c
	@${CC} -c $< -o $@ ${TEST_CFLAGS}


test: build/tests/runner
	@./build/tests/runner $(TEST)


clean:
	@rm -f build/*.o\
	       build/*.d\
	       build/*.a\
	       build/runner\
	       build/nuvm/*.o\
	       build/nuvm/*.d\
	       build/tested/*.o\
	       build/tested/*.d\
	       build/atest/*.o\
	       build/atest/*.d\
	       build/tests/*.o\
	       build/tests/*.d\
	       build/tests/runner

