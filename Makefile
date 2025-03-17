
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra


TARGET = differentiator


SRCS = differentiator.cpp
OBJS = $(SRCS:.cpp=.o)


TEST_DIR = tests
TEST_SRCS = $(TEST_DIR)/test_main.cpp $(TEST_DIR)/test_cases.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_EXEC = test_runner


all: $(TARGET)


$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lm


differentiator_test.o: differentiator.cpp
	$(CXX) $(CXXFLAGS) -DTESTING -c differentiator.cpp -o differentiator_test.o


test: $(TEST_OBJS) differentiator_test.o
	$(CXX) $(CXXFLAGS) -o $(TEST_EXEC) $(TEST_OBJS) differentiator_test.o -lm


run_tests: test
	./$(TEST_EXEC)


clean:
	rm -f $(TARGET) $(TEST_EXEC) $(OBJS) $(TEST_OBJS) differentiator_test.o


.PHONY: all test run_tests clean
