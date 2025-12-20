CC = g++
CFLAGS = -Wall -O2 -mwindows
LDFLAGS = -luser32 -lkernel32 -lwinmm -lstdc++
WINDRES = windres
RES = auto_clicker.res

TARGET = auto_clicker.exe
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC) $(RES)
	$(CC) $(CFLAGS) $(SRC) $(RES) -o $(TARGET) $(LDFLAGS)

$(RES): auto_clicker.rc
	$(WINDRES) auto_clicker.rc -O coff -o $(RES)

clean:
	rm -f $(TARGET) $(RES)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run