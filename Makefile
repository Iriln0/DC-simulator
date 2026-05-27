CXX = g++
CXX_FLAGS = -std=c++17 -Wall -Wextra
SRC = ./src/main.cpp
TARGET = spice

TESTCASES = $(wildcard ./testcase/*.cir)

.PHONY: clean test

$(TARGET): $(SRC)
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(SRC)

test: $(TARGET)
	@mkdir -p testoutput
	@for f in $(TESTCASES); do \
		base=$$(basename $$f .cir); \
		echo "======= Running $$f ======="; \
		./$(TARGET) $$f testoutput/$$base.out; \
	done
	echo "Test Completed";

clean: 
	rm -f $(TARGET)
	rm -rf testoutput