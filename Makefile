# Define compiler and compile options
CXX=g++
CXXFLAGS=-std=c++20 -O2 -Wall

# Define targets and dependencies
TARGET=TPRA
SRCS=src/ensemble.cc src/Fasta.cc
OBJS=$(SRCS:.cc=.o)

# Link object files
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compile the source code
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(TARGET) $(OBJS)
