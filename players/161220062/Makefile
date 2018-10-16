TARGET = connect
CPP = gcc
CFLAGS = -O0 -pthread -w
.PHONY : build run clean

build: $(TARGET).c
	$(CPP) $(CFLAGS) $(TARGET).c -o $(TARGET)

run: build
	@./$(TARGET)

clean:
	rm -f $(TARGET) a.txt eval.py
