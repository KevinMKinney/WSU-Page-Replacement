.SUFFIXES: .c .o
CCFLAGS = -std=c99 -pedantic -Wall -Werror
EXEC = simulate
OPTIONS = -g

build:
	gcc ${CCFLAGS} ${OPTIONS} -o ${EXEC}  pageReplacement.c

clean:
	rm -f ${EXEC}

run: ${EXEC}
	./${EXEC}

# to run valgrind/check for memory problems run the following:
# valgrind --track-origins=yes --leak-check=full ./"my-file-name" "args..."
