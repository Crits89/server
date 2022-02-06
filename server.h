#ifndef __SERVER__
#define __SERVER__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h>


	int socket_desc, client_test, c , c_client=0, next_id=0;
	struct sockaddr_in server , client;
	struct Object *MassObj;
struct Object
{
	int id = 0xFF;
	int id_addres = 0xFF;
	bool pult=false;
	int discript;
	char buffO[512];
	char buffI[512];
	bool autorez=false;
};
struct protocolTCP
{
	uint8_t Obj; //pult or point
	uint16_t pack_lenth; 
	uint8_t id; 
	uint8_t command;
	
	uint8_t data[512-7];
	uint16_t CRC;
};

#endif 