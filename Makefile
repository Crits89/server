all: server.elf client_arm.elf client.elf
server.elf: server.cpp
	gcc -std=gnu++11 -Wall -g -o server.elf server.cpp -lpthread
client.elf: client.cpp
	gcc -std=gnu++11 -Wall -g -o client.elf client.cpp -lpthread
client_arm.elf: client.cpp
	/home/crits/programming/CrossCompiler/orange_pi_zero/gcc-linaro-5.4.1-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc -std=gnu++11 -o client_arm.elf client.cpp --sysroot=/home/crits/mnt/ -lpthread & scp client_arm.elf orangepi@192.168.0.110:~/
clean: *.elf
	rm -rf *.elf