all:
	g++ -g main.cpp -o main -I "./include" -lsfml-window -lsfml-system -lsfml-graphics
	./main
