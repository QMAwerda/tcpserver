# .DEFAULT_GOAL = CC

# .PHONY: buildCC
# buildCC:
# 	@clang++ --std=c++20 src/main.cpp -o obj -O3 -fsanitize=address,undefined -Wall -Wextra -Werror

# .PHONY: CC
# CC: buildCC
# 	@./obj

CXX       := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb

BIN     := compiled
BIN_EXECUTABLE  := compiled/app
SRC     := src
SRC_CLIENT := client
SRC_SERVER := server

.DEFAULT_GOAL = all

tcpclient: client.o mainclient.o
	@$(CXX) $(CXX_FLAGS) -o $(BIN_EXECUTABLE)/tcpclient $(BIN)/client.o $(BIN)/mainclient.o

tcpserver: server.o mainserver.o
	@$(CXX) $(CXX_FLAGS) -o $(BIN_EXECUTABLE)/tcpserver $(BIN)/server.o $(BIN)/mainserver.o

mainclient.o: $(SRC)/$(SRC_CLIENT)/mainclient.cpp
	@$(CXX) $(CXX_FLAGS) -c $(SRC)/$(SRC_CLIENT)/mainclient.cpp -o $(BIN)/mainclient.o

mainserver.o: $(SRC)/$(SRC_SERVER)/mainserver.cpp
	@$(CXX) $(CXX_FLAGS) -c $(SRC)/$(SRC_SERVER)/mainserver.cpp -o $(BIN)/mainserver.o

server.o: $(SRC)/$(SRC_SERVER)/v1/server.cpp $(SRC)/$(SRC_SERVER)/v1/server.hpp
	@$(CXX) $(CXX_FLAGS) -c $(SRC)/$(SRC_SERVER)/v1/server.cpp -o $(BIN)/server.o

client.o: $(SRC)/$(SRC_CLIENT)/v1/client.cpp $(SRC)/$(SRC_CLIENT)/v1/client.hpp
	@$(CXX) $(CXX_FLAGS) -c $(SRC)/$(SRC_CLIENT)/v1/client.cpp -o $(BIN)/client.o

makedir:
	@mkdir $(BIN)
	@mkdir $(BIN_EXECUTABLE)

clean:
	@echo "\nAll compiled files all deleted. Write: 'make' to compile"
	@rm -rf $(BIN)

all: makedir tcpserver tcpclient
	@echo "\nAll files are compiled. Write: 'make startserver' / 'make startclient' to launch"
	@echo "Write 'make clean' to delete all compiled files"

startserver:
	@./$(BIN_EXECUTABLE)/tcpserver

startclient:
	@./$(BIN_EXECUTABLE)/tcpclient
