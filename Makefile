all: server.elf client_arm.elf client.elf
server.elf: server.cpp
	gcc -std=gnu++11 -Wall -g -o server.elf server.cpp -lpthread
client.elf: client.cpp
	gcc -std=gnu++11 -Wall -g -o client.elf client.cpp -lpthread
clean: *.elf
	rm -rf *.elf