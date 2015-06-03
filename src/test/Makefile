CC = gcc

CFLAGS = -g -Wall 

HEADER_DIR = ../headers

#BUILD_DIR = ../../build

#OBJ_DIR = $(BUILD_DIR)/obj
OBJ_DIR = .
OBJS_DIR = ./*.o $(filter-out ../../build/obj/search_6.o, $(wildcard ../../build/obj/*.o))

#BIN_DIR = $(BUILD_DIR)/bin
BIN_DIR = .

INCLUDES = -I$(HEADER_DIR)

MKDIR = mkdir -p

SRCS = $(shell ls *.c)

OBJS = $(SRCS:.c=.o)

TEST = test_clique_count

all : $(TEST)

$(TEST) : $(OBJS) 
	$(CC) $(CFLAGS) -o $(TEST) $(OBJS_DIR)

#dirs:
#	$(MKDIR) $(OBJ_DIR) $(BIN_DIR)

%.o: %.c $(HEADER_DIR)/*.h 
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) -c $< -o $(OBJ_DIR)/$@

clean:
	rm -rf *.o test
