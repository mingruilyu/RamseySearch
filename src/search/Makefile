CC = gcc
CFLAGS = -g -Wall 
HEADER_DIR = ../headers
LOG_DIR = ../../CounterExamples
BUILD_DIR = ../../build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
INCLUDES = -I$(HEADER_DIR)
MKDIR = mkdir -p
SRCS = $(shell ls *.c)
OBJS = $(SRCS:.c=.o)

all: dirs $(OBJS)

dirs:
	$(MKDIR) $(OUT_DIR)	$(OBJ_DIR) $(BIN_DIR)

%.o: %.c $(HEADER_DIR)/*.h 
	$(CC) $(CFLAG) $(INCLUDES) $(LIBS) -c $< -o $(OBJ_DIR)/$@

clean:
	rm -rf $(BUILD_DIR) $(LOG_DIR)
