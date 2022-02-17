#ifndef __SERVER__
#define __SERVER__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h>
#include<sqlite3.h>

	int socket_desc, client_test, c , c_client=0, next_id=0;
	struct sockaddr_in server , client;
	struct Object *MassObj;
struct protocolTCP
{
	uint8_t Obj; //pult or point
	uint16_t pack_lenth; 
	uint8_t id; 
	uint8_t command;
	uint8_t data[256-7];
};
struct Object
{
	int id = 0xFF;
	int id_addres = 0xFF;
	bool pult=false;
	int discript;
	char buffI[256];
	bool autorez=false;
	struct protocolTCP trnsmitter;
	struct protocolTCP *reciver;
};


#endif 