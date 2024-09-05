CC = gcc
OBJS = assembler.o
TARGET = assembler
 
.SUFFIXES : .c .o
 
all : $(TARGET)
 
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) -g -fsanitize=address
 
clean :
	rm -f $(OBJS) $(TARGET)
