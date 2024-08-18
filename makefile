CXXFLAGS = -Iheader -Wall -Wextra -Ofast -Werror $(DEFFLAGS)
DEBUGCXXFLAGS = -Iheader -Wall -Wextra -g -Werror -fsanitize=address -fsanitize=undefined -DCERR_ENABLED
TARGET = Bastet
DEBUGTARGET = Bastet_Debug
SRCS = $(wildcard src/*.cpp)
HEADERS = $(wildcard header/*.h)
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))
DEBUGOBJS = $(patsubst src/%.cpp,obj/%_Debug.o,$(SRCS))

FLAGS_FILE = .flags
TMP_FLAGS_FILE = .tmpflags

all: dirs check_flags $(TARGET)

dirs:
	mkdir -p obj
	mkdir -p build

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o build/$@ $^

$(DEBUGTARGET): $(DEBUGOBJS)
	$(CXX) $(DEBUGCXXFLAGS) -o build/$@ $^

obj/%.o: src/%.cpp $(HEADERS) $(FLAGS_FILE)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

obj/%_Debug.o: src/%.cpp $(HEADERS)
	$(CXX) $(DEBUGCXXFLAGS) -c -o $@ $<

$(FLAGS_FILE):
	@echo $(CXXFLAGS) > $(FLAGS_FILE)

check_flags:
	@echo $(CXXFLAGS) > $(TMP_FLAGS_FILE)
	@if [ -f $(FLAGS_FILE) ]; then \
		if ! cmp -s < $(TMP_FLAGS_FILE) $(FLAGS_FILE); then \
			echo "Compiler flags changed, forcing recompilation..."; \
			rm -f $(OBJS); \
			mv $(TMP_FLAGS_FILE) $(FLAGS_FILE); \
		fi \
	fi

clean:
	rm -f build/*
	rm -f obj/*.o
	rm -f $(FLAGS_FILE)
