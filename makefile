CC = gcc
CFLAGS = -Wall -g
OBJ = Q1.o definitions.o
TARGET = q1

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

Q1.o: Q1.c header.h
	$(CC) $(CFLAGS) -c Q1.c

definitions.o: definitions.c header.h
	$(CC) $(CFLAGS) -c definitions.c

clean:
	rm -f $(OBJ) $(TARGET)
