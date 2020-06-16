# Exec file name
TARGET = lab4

# Plot file
PLOT = results.gp

# cc invoking
CC = mpicc

# user includes
INCLUDE = -Iinclude

# object file names
OBJECTS = src/main.o

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
	$(CC) -c $< -o $@
plot:
	gnuplot ./$(PLOT)
clean:
	rm -rf $(TARGET) $(SRCS)*.o $(PLOT)
