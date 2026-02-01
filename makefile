all: main.cpp src/pai.cc src/tree.cc
	mkdir -p bin
	g++ -std=c++11 -I .\include main.cpp src\pai.cc src\tree.cc -o bin/dou.exe
clean: 
	del /q bin
run: all
# 	bin\dou.exe < input.txt
	bin\dou.exe