all: multifork_demo

multifork_demo: Splitter.o multifork.o
	g++ -o multifork_demo multifork.o Splitter.o

Splitter.o: Splitter.hpp Splitter.cpp
	g++ -c Splitter.cpp

multifork.o:
	g++ -c multifork.cpp

clean:
	rm -f *.o multifork_demo

