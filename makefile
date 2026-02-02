all: main.cpp src/pai.cc src/tree.cc
	g++ -std=c++11 -I .\include main.cpp src\pai.cc src\tree.cc -o bin/dou.exe
clean: 
	del bin\dou.exe
run: all
# 	bin\dou.exe < input.txt
	bin\dou.exe