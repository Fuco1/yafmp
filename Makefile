CXX = gcc
CXXFLAGS = -Wall -std=c99 -ggdb

OBJ_DIR = bin
LIB_DIR = -L/usr/local/lib
INC_DIR = -I/usr/local/include

LDLIBS = $(LIB_DIR)

SOURCE = iarray.c group.c heatmap.c main.c
OBJECTS = ${SOURCE:%.c=$(OBJ_DIR)/%.o}
EXECUTABLE = main

all: init $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

$(OBJ_DIR)/%.o: %.c
	$(CXX) $(CXXFLAGS) $(INC_DIR) -c $< -o $@

init:
	@mkdir -p "$(OBJ_DIR)"

clean:
	@rm -rf $(OBJ_DIR) $(EXECUTABLE)
