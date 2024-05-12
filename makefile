CXX = g++
CXXFLAGS = -Iheader -Wall -Wextra -Ofast
DEBUGCXXFLAGS = -Iheader -Wall -Wextra -g -fsanitize=address -fsanitize=undefined
TARGET = Bastet
DEBUGTARGET = Bastet_Debug
SRCS = $(wildcard src/*.cpp)
HEADERS = $(wildcard header/*.h)
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))
DEBUGOBJS = $(patsubst src/%.cpp,obj/%_Debug.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o build/$@ $^

$(DEBUGTARGET): $(DEBUGOBJS)
	$(CXX) $(DEBUGCXXFLAGS) -o $@ $^

obj/%.o: src/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

obj/%_Debug.o: src/%.cpp $(HEADERS)
	$(CXX) $(DEBUGCXXFLAGS) -c -o $@ $<

clean:
	rm $(TARGET)*
	rm obj/*.o