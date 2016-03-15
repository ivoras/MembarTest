benchmark: benchmark.cpp
	g++ -O1 -o benchmark -pthread -std=c++11 -Wall -pedantic benchmark.cpp
	g++ -O1 -S -o benchmark.S -std=c++11 -Wall -pedantic -c benchmark.cpp

clean:
	rm -f benchmark benchmark.exe
