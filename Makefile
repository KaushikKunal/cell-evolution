all: compile link run

compile:
	g++ -c src/main.cpp -o bin/main.o -Isrc/sfml/include --std=c++11

link:
	g++ bin/main.o -o bin/main -Isrc/sfml/include -Lsrc/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system
	
run:
	export LD_LIBRARY_PATH=src/sfml/lib && ./bin/main