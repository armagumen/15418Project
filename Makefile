test: test_harness.cpp utils/testing.o
	g++ test_harness.cpp utils/testing.o -o test
	./test

utils/testing.o: utils/testing.h utils/testing.cpp
	g++ -o utils/testing.o -c utils/testing.cpp
