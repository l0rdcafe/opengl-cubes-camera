CC=g++
CFLAGS=-I/usr/local/include -Wall
FILES=main.cpp glad.c
OUTPUT=main
LIBS=-L/usr/local/Cellar/glfw/3.3.2/lib/ -lglfw.3 -L/usr/local/Cellar/glm/0.9.9.8/lib/

main: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o $(OUTPUT) $(LIBS)

clean:
	rm -rf *.o $(OUTPUT)

all: main
