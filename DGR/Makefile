exe = CSR_Reader

cc = "$(shell which g++)" 

graph.o: graph.cpp graph.h
	$(cc) -c graph.cpp

main.o: main.cpp
	$(cc) -c main.cpp

$(exe):graph.o main.o
	$(cc) -o main graph.o main.o

clean:
	rm -rf $(exe) *.o