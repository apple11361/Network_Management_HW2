CC = gcc -std=c99
CCFLAGS = -Wall -O3

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS)
	$(CC) $(CCFLAGS) -o main main.c typedefine.h parser.c error_checking.c signal_converter.c

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

parse:
	./main
	./main > out.txt
compare:
	diff out.txt test_case/output.txt
clean:
	rm -rf main out.txt *.o
