# .DEFAULT_GOAL = CC

# .PHONY: buildCC
# buildCC:
# 	@clang++ --std=c++20 src/main.cpp -o obj -O3 -fsanitize=address,undefined -Wall -Wextra -Werror

# .PHONY: CC
# CC: buildCC
# 	@./obj

tcpserver: server.o client.o main.o
	g++ -o compiled/tcpserver compiled/server.o compiled/client.o compiled/main.o

main.o: src/main.cpp
	g++ -c src/main.cpp -o compiled/main.o

server.o: src/server.cpp src/server.hpp
	g++ -c src/server.cpp -o compiled/server.o

client.o: src/client.cpp src/client.hpp
	g++ -c src/client.cpp -o compiled/client.o

all: tcpserver
	./compiled/tcpserver