main: main.cpp
	g++ main.cpp -std=c++20 -o out 

clean: out
	rm out
