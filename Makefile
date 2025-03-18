CC = gcc
CFLAGS = -Wall -Wextra -lncurses
TARGET = dash
SRC = src/geometry_dash.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
