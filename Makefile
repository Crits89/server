all: server.elf client.elf
server.elf: server.cpp
	gcc -Wall -g -o server.elf server.cpp -lpthread
client.elf: client.cpp
	gcc -Wall -g -o client.elf client.cpp
clean: *.elf
	rm -rf *.elf