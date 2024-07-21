# Install

BIN = snake2

# Flags
CFLAGS += -Os -s -static -municode -mwindows -march=core2

SRC =  src/main.c src/logic.c src/winproc.c src/winio.c app.res icon.res
OBJ = $(SRC:.c=.o)

BIN := $(BIN).exe
LIBS = -lgdi32

$(BIN):
	@mkdir -p bin
	rm -f bin/$(BIN) $(OBJS)
	$(CC) $(SRC) $(CFLAGS) -o bin/$(BIN) $(LIBS)
	cp snake.lng bin/snake.lng
