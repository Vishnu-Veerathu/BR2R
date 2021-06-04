CC = g++

LIBS=-lm

tog_classifier: Makefile tog_classifier.o
	${CC} -g ${CFLAGS} -o tog_classifier tog_classifier.o graph.o ${LIBS}

tog_classifier.o: Makefile br2finder.cpp graph.o
	${CC} ${CFLAGS} -c br2finder.cpp -o tog_classifier.o

graph.o: graph.cc graph.h
	${CC} ${CFLAGS} -c graph.cc

result_counter: Makefile result_counter.o
	${CC} -g ${CFLAGS} -o result_counter result_counter.o

result_counter.o: Makefile result_counter.cpp
	${CC} ${CFLAGS} -c result_counter.cpp -o result_counter.o

clean: 
	rm *.o
