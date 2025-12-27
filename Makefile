CXX = g++
CXXFLAGS = -std=c++17 -Wall `pkg-config gtkmm-3.0 --cflags`
LDFLAGS = `pkg-config gtkmm-3.0 --libs`

# List of source files
SRCS = main.cpp Board.cpp Tetromino.cpp AI.cpp GameArea.cpp

# List of object files
OBJS = $(SRCS:.cpp=.o)

# The final executable
TARGET = tetris_ai

# Default rule
all: $(TARGET)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule to compile a .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for cleaning up the build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
