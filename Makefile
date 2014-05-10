CC=clang

CFLAGS=-pedantic -Wall -std=c89 -g
LIBS=

NUVM_ASM_SOURCE=$(wildcard asm/*.c)
NUVM_ASM_OBJS=$(NUVM_ASM_SOURCE:asm/%.c=build/asm/%.o)
NUVM_ASM_CFLAGS=$(CFLAGS)

NUVM_COMMON_SOURCE=$(wildcard common/*.c)
NUVM_COMMON_OBJS=$(NUVM_COMMON_SOURCE:common/%.c=build/common/%.o)
NUVM_COMMON_CFLAGS=$(CFLAGS)

TEST_SOURCE=$(wildcard tests/suites/*.c)
TEST_OBJS=$(TEST_SOURCE:test/suites/%.c=build/tests/%.o)
TEST_CFLAGS=$(CFLAGS)

all: build/asm/libnuvm-asm.a build/common/libnuvm-common.a


# Used for automatic header dependency tracking generated by the compiler.
-include $(patsubst %.o, %.d, $(NUVM_ASM_OBJS))


# The actual final artifacts we're building, separated in static libraries.
build/asm/libnuvm-asm.a: build/common/libnuvm-common.a $(NUVM_ASM_OBJS) 
	@ar rcs $@ $(NUVM_ASM_OBJS)

build/common/libnuvm-common.a: $(NUVM_COMMON_OBJS)
	@ar rcs $@ $^


# How to build the object files to be tested; the -MM -MT line invokes the
# compiler's automatic header file dependency tracking for object files.
build/common/%.o: common/%.c
	@${CC} -c $< -o $@ $(NUVM_COMMON_CFLAGS)
	@${CC} -MM -MT $@ $(NUVM_COMMON_CFLAGS) $< > $(@:.o=.d)

build/asm/%.o: asm/%.c
	@${CC} -c $< -o $@ $(NUVM_ASM_CFLAGS)
	@${CC} -MM -MT $@ $(NUVM_ASM_CFLAGS) $< > $(@:.o=.d)


# Clean up after ourselves.
clean:
	@rm -f $(shell find build/ -name '*.o' -o -name '*.a' -o -name '*.d')
