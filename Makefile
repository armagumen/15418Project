test: test_harness.cpp utils/testing.o utils/types.h
	g++ test_harness.cpp utils/testing.o -o test
	./test

utils/testing.o: utils/testing.h utils/testing.cpp utils/types.h
	g++ -o utils/testing.o -c utils/testing.cpp 
