all:
	g++ -g main.cpp -o main -I "./include" -lsfml-graphics -lsfml-system -lsfml-window
	./main
