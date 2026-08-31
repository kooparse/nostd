EXEC_FILEPATH := ./build/test

compile:
	c++ -std=c++20 -O0 -Wall -Wextra test.cpp -o $(EXEC_FILEPATH)
run:
	c++ -std=c++20 -O0 -Wall -Wextra test.cpp -o $(EXEC_FILEPATH) && $(EXEC_FILEPATH)
