EXEC_FILEPATH := ./build/example

compile:
	c++ -std=c++20 -O0 -Wall -Wextra example.cpp -o $(EXEC_FILEPATH)
run:
	c++ -std=c++20 -O0 -Wall -Wextra example.cpp -o $(EXEC_FILEPATH) && $(EXEC_FILEPATH)
