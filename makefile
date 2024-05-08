CXX = g++
CXXFLAGS = -Iheader -Wall -Wextra -Ofast
TARGET = Bastet_V2
SRCS = $(wildcard src/*.cpp)
HEADERS = $(wildcard header/*.h)
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


obj/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm $(TARGET)
	rm obj/*.o