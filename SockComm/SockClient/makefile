CC = gcc
CFLAGS = -pthread -g
HEADER_DIR = ../Headers
INCLUDES = -I$(HEADER_DIR)
EXENAME = Client

$(EXENAME): $(EXENAME).o
	$(CC) $(CFLAGS) -o $(EXENAME) $(EXENAME).o

%.o: %.c $(HEADER_DIR)/Transfer.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(EXENAME) $(EXENAME).o
