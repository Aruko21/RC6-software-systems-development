# Exec file name
TARGET = lab3

# cc invoking
CC = gcc

# user includes
INCLUDE = -Iinclude

# object file names
OBJECTS = src/main.o \
	src/buffers.o \
	src/parsers.o \
	src/http_client.o

# source files directory
SRCS = src/

# project dependence
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

# $< means first dependent argument. Example: all: obj obj2 - $< = obj
# $@ means target. For previous example this is "all"
# rule for implicit handling of all object files
%.o: %.c
	$(CC) -c $< -o $@ $(INCLUDE)
clean:
	rm -rf $(TARGET) $(SRCS)*.o
