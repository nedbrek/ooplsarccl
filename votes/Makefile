all: debug.exe main.exe

main.exe: votes.cpp
	@g++-4.8 -o$@ -std=c++11 -Wall $^

debug.exe: votes.cpp
	@g++-4.8 -o$@ -DOUTPUT -std=c++11 -Wall $^

test: main.exe
	@./main.exe < sad.in > test.out
	@diff sad.out test.out

