# Exec file name
TARGET = lab1
TARGET_MOD = lab1-mod

# cc invoking
CC = gcc

# object file names
OBJECTS = src/main.o

OBJECTS_MOD = src/main-modified.o

SRCS = src/

# project dependence
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

$(TARGET_MOD): $(OBJECTS_MOD)
	$(CC) -o $(TARGET_MOD) $(OBJECTS_MOD)

clean:
	rm -rf ${TARGET} ${TARGET_MOD} ${SRCS}*.o
