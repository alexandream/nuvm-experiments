LIBS=-L ../build/ -lnuvm `pkg-config --libs check`
CFLAGS=-std=c11 `pkg-config --cflags check` -I ../src/ -g
CC=clang

RUN_SUITE=                                         \
	echo "";                                       \
	${CC} -o runner $(subst -, ,$(2)) ${LIBS};     \
	./runner "$(1)"

all: clean\
	 Value_Representation\
	 Behaviour

runner.o: runner.c
	@${CC} -c runner.c -o runner.o ${CFLAGS}

cases/%.o: cases/%.c
	@${CC} -c $< -o $@ ${CFLAGS}


clean:
	@rm cases/*.o runner.o runner -f

Value_Representation: runner.o\
					  cases/value.o\
					  cases/module.o\
					  cases/primitive.o\
					  cases/procedure.o\
					  cases/instructions.o
	@$(call RUN_SUITE,$@,$^)

Behaviour: runner.o\
	       cases/evaluator.o
	@$(call RUN_SUITE,$@,$^)

