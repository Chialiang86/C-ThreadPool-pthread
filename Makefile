CC = gcc
SRC = main.c 
EXEC = picnt

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $^ -lpthread -lm -o $@

rm:
	-rm $(EXEC)
