FILES = $(wildcard *.c)
OBJECTS_DIR = build
OBJECTS_NAME = 
OBJECTS = $(addprefix $(OBJECTS_DIR)/, $(OBJECTS_NAME))

CC = gcc

.PHONY : clean

all : server

server : $(OBJECTS) $(OBJECTS_DIR)/server.o
	@$(CC) -o server $(OBJECTS) $(OBJECTS_DIR)/server.o

$(OBJECTS) : $(OBJECTS_DIR)/%.o: %.c
	@$(CC) -c $< -o $@

$(OBJECTS_DIR)/server.o : server.c
	@$(CC) -c $< -o $@

clean :
	rm server $(OBJECTS) $(OBJECTS_DIR)/server.o