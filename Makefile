CC := g++ -std=c++11
LIBS := -lsfml-graphics -lsfml-window -lsfml-system -lGL -lm
LIB_FATNOISE := lib/fastNoise/FastNoise.cpp
LIB_ALL := $(LIB_FATNOISE)
EXE_NAME := vox

all: clean vox

vox: main.cpp
	$(CC) -g -o $(EXE_NAME) $(LIB_ALL) fileIO.cpp math.cpp camera.cpp renderable.cpp main.cpp $(LIBS)

clean:
	rm -rf *.o $(EXE_NAME)
