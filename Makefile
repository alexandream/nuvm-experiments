CC=clang

CFLAGS=-pedantic -Wall -std=c89 -g -I "src" -DTEST_ACCESSORS
LIBS=

ATEST_ROOT=../atest/build

NUVM_ASM_SOURCE=$(wildcard src/asm/*.c)
NUVM_ASM_OBJS=$(NUVM_ASM_SOURCE:src/asm/%.c=build/asm/%.o)
NUVM_ASM_CFLAGS=$(CFLAGS)

NUVM_COMMON_SOURCE=$(wildcard src/common/*.c)
NUVM_COMMON_OBJS=$(NUVM_COMMON_SOURCE:src/common/%.c=build/common/%.o)
NUVM_COMMON_CFLAGS=$(CFLAGS)

TEST_CFLAGS=$(CFLAGS) -I $(ATEST_ROOT)
TEST_LIBS=$(LIBS) -L$(ATEST_ROOT) -latest

TEST_COMMON_SOURCE=$(wildcard tests/suites/common/*.c)
TEST_COMMON_OBJS=$(TEST_COMMON_SOURCE:tests/suites/%.c=build/tests/%.o)
TEST_COMMON_CFLAGS=$(TEST_CFLAGS) -I "src/common"
TEST_COMMON_LIBS=$(TEST_LIBS) -Lbuild/common -lnuvm-common

TEST_ASM_SOURCE=$(wildcard tests/suites/asm/*.c)
TEST_ASM_OBJS=$(TEST_ASM_SOURCE:tests/suites/%.c=build/tests/%.o)
TEST_ASM_CFLAGS=$(TEST_CFLAGS) -I "src/asm"
TEST_ASM_LIBS=$(TEST_LIBS) -Lbuild/asm -lnuvm-asm $(TEST_COMMON_LIBS)
all: build/asm/libnuvm-asm.a build/common/libnuvm-common.a


# Used for automatic header dependency tracking generated by the compiler.
-include $(patsubst %.o, %.d, $(NUVM_ASM_OBJS))
-include $(patsubst %.o, %.d, $(NUVM_COMMON_OBJS))


# The actual final artifacts we're building, separated in static libraries.
build/asm/libnuvm-asm.a: build/common/libnuvm-common.a $(NUVM_ASM_OBJS)
	@ar rcs $@ $^

build/common/libnuvm-common.a: $(NUVM_COMMON_OBJS)
	@ar rcs $@ $^



# How to build the object files to be tested; the -MM -MT line invokes the
# compiler's automatic header file dependency tracking for object files.
build/common/%.o: src/common/%.c
	@${CC} -c $< -o $@ $(NUVM_COMMON_CFLAGS)
	@${CC} -MM -MT $@ $(NUVM_COMMON_CFLAGS) $< > $(@:.o=.d)

build/asm/%.o: src/asm/%.c
	@${CC} -c $< -o $@ $(NUVM_ASM_CFLAGS)
	@${CC} -MM -MT $@ $(NUVM_ASM_CFLAGS) $< > $(@:.o=.d)


# How to build the test targets for each module
build/tests/%.o: tests/%.c
	@$(CC) -c $< -o $@ $(TEST_CFLAGS)

build/tests/run-common: $(TEST_COMMON_OBJS) \
                        build/common/libnuvm-common.a \
                        build/tests/runner.o \
						build/tests/test-suite.o \
                        build/tests/run-common.o
	@$(CC) -o build/tests/run-common $^ $(TEST_COMMON_LIBS)

build/tests/run-asm: $(TEST_ASM_OBJS) \
                     build/asm/libnuvm-asm.a \
                     build/common/libnuvm-common.a \
                     build/tests/runner.o \
					 build/tests/test-suite.o \
                     build/tests/run-asm.o
	@$(CC) -o build/tests/run-asm $^ $(TEST_LIBS) $(TEST_ASM_LIBS)


# How to build each of the test suites.
build/tests/common/%.o: tests/suites/common/%.c
	@$(CC) -c $< -o $@ $(TEST_COMMON_CFLAGS)

build/tests/asm/%.o: tests/suites/asm/%.c
	@$(CC) -c $< -o $@ $(TEST_ASM_CFLAGS)


# Auxiliary targets that builds and tests everything, takes an ENV var so that
# I can sometimes say which specific test to run, instead of running the full
# battery of tests.
test: build/tests/run-common build/tests/run-asm
	@echo Running Common Tests
	@./build/tests/run-common $(TEST)
	@echo
	@echo Running Assembler Tests
	@./build/tests/run-asm $(TEST)

# Clean up after ourselves.
clean: FORCE
	@rm -f $(shell find build/ -name '*.o' -o -name '*.a' -o -name '*.d') .bogus
	@rm -f $(shell find build/ -type f -a -executable) .bogus

FORCE:
