CC = gcc
CFLAGS = -Wall -Os -I../kirk -I../common
TARGET = PrxEncrypter
OBJS = crypto.o kirk_engine.o main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
	$(RM) *.o $(TARGET) *.exe *.exe.stackdump
