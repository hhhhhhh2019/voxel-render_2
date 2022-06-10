all:
	g++ -g main.cpp glad.c -o main -I "./include" -lOpenCL -ldl -lglfw -lGL
	./main
