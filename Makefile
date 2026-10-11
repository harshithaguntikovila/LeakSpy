CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = leakspy

SRC = leakspy.c test_program.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
