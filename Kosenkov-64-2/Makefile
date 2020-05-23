# Exec file name
TARGET = lab2
TARGET_DEBUG = debug

# cc invoking
CC = gcc

# extern libraries
LIBS = -lpthread

STANDARD = gnu99
# user includes
INCLUDE = -Iinclude

# object file names
OBJECTS = src/main.o \
	src/main_thread.o \
	src/first_thread.o \
	src/second_thread.o \
	src/string_handlers.o

OBJECTS_DEBUG = src/main.o \
	src/main_thread_debug.o \
	src/first_thread.o \
	src/second_thread.o \
	src/string_handlers.o

# source files directory
SRCS = src/

# project dependence
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(LIBS)

$(TARGET_DEBUG): $(OBJECTS_DEBUG)
	$(CC) -o $(TARGET)_$(TARGET_DEBUG) $(OBJECTS_DEBUG) $(LIBS)

# $< means first dependent argument. Example: all: obj obj2 - $< = obj
# $@ means target. For previous example this is "all"
# rule for implicit handling of all object files
%.o: %.c
	$(CC) -c $< -o $@ $(INCLUDE) -std=$(STANDARD)

clean:
	rm -rf $(TARGET) $(TARGET_MOD) $(SRCS)*.o
