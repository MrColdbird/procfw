TARGET=libpspkubridge.a
all: $(TARGET)
OBJS = KUBridge_0000.o KUBridge_0001.o KUBridge_0002.o KUBridge_0003.o KUBridge_0004.o KUBridge_0005.o KUBridge_0006.o KUBridge_0007.o KUBridge_0008.o KUBridge_0009.o KUBridge_0010.o KUBridge_0011.o KUBridge_0012.o KUBridge_0013.o KUBridge_0014.o KUBridge_0015.o KUBridge_0016.o KUBridge_0017.o 

PSPSDK=$(shell psp-config --pspsdk-path)

clean:
	rm -f $(OBJS) $(TARGET)

CC=psp-gcc
INCDIR = 
CFLAGS = -Os -G0 -Wall -fno-builtin-printf -I$(PSPSDK)/include
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)
LDFLAGS=-nodefaultlibs

LIBDIR =

$(TARGET): $(OBJS)
	psp-ar q $(TARGET) $(OBJS)
	psp-ranlib $(TARGET)

$(OBJS): KUBridge.S
	$(CC) $(CFLAGS) -DF_$* $< -c -o $@

