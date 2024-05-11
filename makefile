CXX = g++
CXXFLAGS = -Iheader -Wall -Wextra -Ofast
DEBUGCXXFLAGS = -Iheader -Wall -Wextra -g
TARGET = Bastet
DEBUGTARGT = Bastet_Debug
SRCS = $(wildcard src/*.cpp)
HEADERS = $(wildcard header/*.h)
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))
DEBUGOBJS = $(patsubst src/%.cpp,obj/%_Debug.o,$(SRCS))

all: $(TARGET)

$(DEBUGTARGT): $(DEBUGOBJS)
	$(CXX) $(DEBUGCXXFLAGS) -o $@ $^ 

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


obj/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<


obj/%_Debug.o: src/%.cpp $(HEADERS)
	$(CXX) $(DEBUGCXXFLAGS) -c -o $@ $<

clean:
	rm $(TARGET)
	rm obj/*.o