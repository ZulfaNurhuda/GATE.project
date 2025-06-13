CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
SRCS = src/main.cpp src/lexer.cpp src/parser.cpp src/generator.cpp src/symbol_table.cpp src/error_handler.cpp src/logger.cpp src/ui.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = gate

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)