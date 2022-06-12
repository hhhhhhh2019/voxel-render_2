all:
	g++ -g main.cpp -o main -I "./include" -lOpenCL -lsfml-graphics -lsfml-system -lsfml-window
	./main
