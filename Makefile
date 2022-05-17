all: server.elf client.elf
server.elf: server.o
	g++ -std=gnu++11 -Wall server.o -o server.elf -lpthread -lsqlite3
server.o: server.cpp
	g++ -c server.cpp 
client.elf: client.cpp
	g++ -std=gnu++11 -Wall -o client.elf client.cpp -lpthread
clean:
	rm -rf *.elf
	rm -rf *.o
