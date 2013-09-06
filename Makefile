CFLAGS=-Wall -std=c11 -g
CC=clang

SOURCE_FILES=\
	$(wildcard src/*.c src/types/*.c)
OBJ_FILES=\
	$(patsubst src/%.c, build/%.o, $(SOURCE_FILES))

all: $(OBJ_FILES) replicate_build_structure
	@ar rcs build/libnuvm.a build/*.o build/types/*.o

replicate_build_structure:
	`find src/ -type d | sed 's/^src/build/' | xargs mkdir -p`
	

build/%.o: src/%.c
	@${CC} -c $< -o $@ ${CFLAGS}


test: all
	@cd test; ./run-tests.sh

build/runner: all
	@${CC} -o build/runner runner.c -L build/ -lnuvm ${CFLAGS} -I src/

run: build/runner
	@build/runner

clean:
	@rm -f build/*.a build/*.o build/runner build/types/*.o
	@cd test; make clean -f Makefile
