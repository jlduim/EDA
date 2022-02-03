main: main.o structs.o funcs.o
	g++ main.o structs.o funcs.o -o main -O2 -ljsoncpp

prepareJson: prepareJson.o
	g++ prepareJson.o -O2 -Wall -ljsoncpp -o prepareJson

testes: testes.o funcs.o structs.o
	g++ testes.o funcs.o structs.o -o testes -ljsoncpp

servidor: servidor.o funcs.o structs.o
	g++ servidor.o funcs.o structs.o -O2 -Wall -lmicrohttpd -ljsoncpp -o servidor

main.o: main.cpp
	g++ -c main.cpp -O2 -Wall

structs.o: structs.cpp
	g++ -c structs.cpp -O2 -Wall

funcs.o: funcs.cpp
	g++ -c funcs.cpp -O2 -Wall

prepareJson.o: prepareJson.cpp
	g++ -c prepareJson.cpp

servidor.o: servidor.cpp
	g++ -c servidor.cpp

testes.o: testes.cpp
	g++ -c testes.cpp

clear:
	rm *.o