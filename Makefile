CC := g++ -std=c++11
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lGL -lm
LIB_FATNOISE := lib/fastNoise/FastNoise.cpp
LIB_ALL := $(LIB_FATNOISE)

all: clean vox

vox: main.cpp
	$(CC) -g -o vox $(LIB_ALL) math.cpp camera.cpp renderable.cpp main.cpp $(LIBS)

clean:
	rm -rf *.o vox
