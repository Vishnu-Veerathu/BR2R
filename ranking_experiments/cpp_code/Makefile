CC = g++

LIBS=-lm

realtest: Makefile realtest.o
	${CC} -g ${CFLAGS} -o realtest realtest.o graph.o ./OptSpace_C/libOptSpace.a ${LIBS}

synthtest: Makefile synthtest.o
	${CC} -g ${CFLAGS} -o synthtest synthtest.o graph.o ./OptSpace_C/libOptSpace.a ${LIBS}

realtest.o: Makefile real_data_test.cpp graph.o
	${CC} ${CFLAGS} -c real_data_test.cpp -o realtest.o

synthtest.o: Makefile synth_data_test.cpp graph.o
	${CC} ${CFLAGS} -c synth_data_test.cpp -o synthtest.o

graph.o: graph.cc graph.h
	${CC} ${CFLAGS} -c graph.cc

clean: 
	rm *.o