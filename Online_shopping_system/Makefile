CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = shopping_system
SRC      = main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)
	@echo "Build successful. Run: ./$(TARGET)"

clean:
	rm -f $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
