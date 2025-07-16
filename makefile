# List of MAX_ORDER values
ORDERS := 1 2 3 4 5 6 7

# Source files
SRCS := main.cpp Scheme.cpp Rank1Tensor.cpp

# Header dependencies (not compiled directly)
HEADERS := Scheme.h Rank1Tensor.h

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall

# Disable built-in rules to always rebuild
MAKEFLAGS += --always-make

# Targets
all: $(ORDERS:%=flip%)

# Pattern rule for flip1, flip2, ..., flip7
flip%: $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) -DMAX_ORDER=$* $(SRCS) -o $@

# Clean up
clean:
	rm -f flip1 flip2 flip3 flip4 flip5 flip6 flip7

