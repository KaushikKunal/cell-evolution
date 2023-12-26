all: compile link run

compile:
	g++ -c main.cpp -Isfml/include --std=c++11

link:
	g++ main.o -o sfml_test -Isfml/include -Lsfml/lib -lsfml-graphics -lsfml-window -lsfml-system
	
run:
	export LD_LIBRARY_PATH=sfml/lib && ./sfml_test