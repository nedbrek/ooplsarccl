CXX := g++-4.8
CXXFLAGS += -pedantic -Wall -std=c++11

BIN := kitayuta.exe

.PHONY: all
all: $(BIN)

$(BIN): kitayuta.cpp
	@$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: test
test: $(BIN)
	@./$(BIN) < test.in > test.tmp
	diff test.gold test.tmp

