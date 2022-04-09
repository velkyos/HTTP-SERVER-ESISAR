FILEDIR = src
OBJDIR = bin

BIN = http-server
CC = gcc
FLAGS = -Wall

C_FILES = $(wildcard $(FILEDIR)/*.c)
OBJ_FILES = $(addprefix $(OBJDIR)/, $(notdir $(C_FILES:.c=.o)))

all: $(OBJ_FILES)
	$(CC) $(FLAGS) -o $(BIN) $^ -L. -lrequest

$(OBJ_FILES): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/main.o: $(FILEDIR)/main.c
	$(CC) $(FLAGS) -g -I./src/message -c -o $@ $^ 

$(OBJDIR)/%.o: $(FILEDIR)/%.c
	$(CC) $(FLAGS) -c -o $@ $^

clean:
	rm -fr $(OBJDIR)/*.o *~ $(BIN)