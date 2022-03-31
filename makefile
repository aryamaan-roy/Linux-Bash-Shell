CC = gcc
CFLAGS  = -g -Wall -lm

TARGET = shell

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c $(CFLAGS)

clean:
	$(RM) $(TARGET)