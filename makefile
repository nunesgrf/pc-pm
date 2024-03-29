CPP   = g++
CC	 = gcc
SRCS	   = $(shell find src -name '*.cpp')
OBJ   = $(addprefix obj/,$(notdir $(SRCS:%.cpp=%.o))) 
LIBS	 = 
INCS	 = 
CXXINCS  = -I"include/"
BIN   = run
CXXFLAGS = $(CXXINCS) -std=c++11 -g3 -ggdb3 -DDEBUG -pthread
CFLAGS   = $(INCS) -std=c11 -ggdb3 -pthread
RM	 = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o $(BIN) $(LIBS) $(CXXFLAGS)

obj/%.o: src/%.cpp
	$(CPP) -c $< -o $@ $(CXXFLAGS)