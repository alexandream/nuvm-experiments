LIBS=-L ../build/ -lnuvm `pkg-config --libs check`
CFLAGS=-std=c11 `pkg-config --cflags check` -I ../src/ -g
CC=clang

RUN_SUITE=                                         \
	echo "";                                       \
	${CC} -o runner $(subst -, ,$(2)) ${LIBS};     \
	./runner "$(1)"

all: clean

runner.o: runner.c
	@${CC} -c runner.c -o runner.o ${CFLAGS}

cases/%.o: cases/%.c
	@${CC} -c $< -o $@ ${CFLAGS}


clean:
	@rm cases/*.o runner.o runner -f

#Dummy: runner.o\
#       cases/dummy.o
#	@$(call RUN_SUITE,$@,$^)

