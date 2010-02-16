        C = gcc
    CFLAGS = -O -pedantic -g 
       OBJ = g_list.o huh_main.o

BINARY = huh

all: ${OBJ}
	${CC} ${CFLAGS} -o ${BINARY} ${OBJ}

huh_main.o: huh_main.c
	${CC} ${CFLAGS} -c huh_main.c

g_list.o: g_list.c g_list.h
	${CC} ${CFLAGS} -c g_list.c
	
clean:
	rm -f *.o ${BINARY}
