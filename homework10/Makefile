TARGET = filemanager

.PHONY: all clean

all: $(TARGET)

clean:
	rm *.o

main.o: main.c
	gcc main.c -c

fmfuncs.o: fmfuncs.c
	gcc fmfuncs.c -c

$(TARGET): main.o fmfuncs.o fmheader.h
	gcc main.o fmfuncs.o -o $(TARGET) -lncurses
