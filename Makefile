# Install

BIN = snake

# Flags
CFLAGS += -Os -s -static -municode -mwindows -march=core2

SRC =  main.c logic.c winproc.c app.res
OBJ = $(SRC:.c=.o)

BIN := $(BIN).exe
LIBS = -lgdi32

$(BIN):
	@mkdir -p bin
	rm -f bin/$(BIN) $(OBJS)
	$(CC) $(SRC) $(CFLAGS) -o bin/$(BIN) $(LIBS)
