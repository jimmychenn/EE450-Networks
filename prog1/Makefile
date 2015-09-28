all: test-server test-client lossless-client lossy-client

test-server: test-server.o
	${CC} -o test-server -g test-server.o -lpthread

test-server.o: test-server.c
	${CC} -g -c -Wall test-server.c -lpthread

test-client: test-client.o
	${CC} -o test-client -g test-client.o

test-client.o: test-client.c
	${CC} -g -c -Wall test-client.c 

your-client: your-client.o
	${CC} -o your-client -g your-client.o

your-client.o: your-client.c
	${CC} -g -c -Wall your-client.c

seg_fault_test: seg_fault_test.o
	${CC} -o seg_fault_test -g seg_fault_test.o

seg_fault_test.o: seg_fault_test.c
	${CC} -g -c -Wall seg_fault_test.c

clean:
	rm -f *.o *~ test-server test-client your-client 