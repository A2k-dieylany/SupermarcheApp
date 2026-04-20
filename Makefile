CXX = g++
CXXFLAGS = -std=c++17 -Wall -Ilib

SRCS = src/main.cpp src/client.cpp src/caisse.cpp src/supermarche.cpp
TARGET = supermarche

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)