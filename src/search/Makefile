CC = gcc
CFLAGS = -g -Wall 
HEADER_DIR = ../headers
LOG_DIR = ../../CounterExamples
BUILD_DIR = ../../build
#BUILD_DIR = .
OBJ_DIR = $(BUILD_DIR)/obj
#OBJ_DIR = .
BIN_DIR = $(BUILD_DIR)/bin
#BIN_DIR = .
INCLUDES = -I$(HEADER_DIR)
MKDIR = mkdir -p
SRCS = $(shell ls *.c)
OBJS = $(SRCS:.c=.o)
OBJS_BUILD = $(shell ls $(OBJ_DIR)/*.o)
APP = prog

all: dirs $(APP)
#all: $(APP)
$(APP): $(OBJS)
	$(CC) $(CFLAG) -o $(BIN_DIR)/$(APP) $(OBJS_BUILD) 
#	$(CC) $(CFLAG) -o $(APP) $(OBJS) 

dirs:
	$(MKDIR) $(OUT_DIR)	$(OBJ_DIR) $(BIN_DIR)

%.o: %.c $(HEADER_DIR)/*.h 
	$(CC) $(CFLAG) $(INCLUDES) $(LIBS) -c $< -o $(OBJ_DIR)/$@
#	$(CC) $(CFLAG) $(INCLUDES) $(LIBS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(LOG_DIR)
#	rm -rf *.o prog
