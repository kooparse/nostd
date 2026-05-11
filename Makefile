EXEC_FILEPATH := ./build/example

compile:
	c++ example.cpp -o $(EXEC_FILEPATH)
run:
	c++ example.cpp -o $(EXEC_FILEPATH) && $(EXEC_FILEPATH)
