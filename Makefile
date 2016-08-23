#Makefile

PRG = mcast_server mcast_client
SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=%.o)
SVR_OBJ = $(OBJ:mcast_client.o=)
CLI_OBJ = $(OBJ:mcast_server.o=)
LIB = -lpthread
FLAG = -g

.PHONY:all
all:$(PRG)

mcast_server:$(SVR_OBJ)
	g++ -o $@ $^ $(FLAG) $(LIB)

mcast_client:$(CLI_OBJ) 
	g++ -o $@ $^ $(FLAG) $(LIB)

%.o:%.cpp
	g++ -c -o $@ $< $(FLAG)

.PHONY:output
output:
	mkdir output
	cp $(PRG) output/
	cp client.cfg output/
	cp server.cfg output/
	tar -czvf mcast.tar.gz output

.PHONY:clean
clean:
	rm $(OBJ) $(PRG)
	rm -rf output
	rm mcast.tar.gz
