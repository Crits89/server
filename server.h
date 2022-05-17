#ifndef __SERVER__
#define __SERVER__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h>
#include<vector>
#include<sqlite3.h>
#include<iostream>
#include "src/SQL_driver.cpp"
#include <time.h>
#include <sys/stat.h>
#include <openssl/sha.h>

// **************************************  DEFINE  ********************************************
#define DATA  0x05
#define SERVER_ID 0x01
#define OBJECT_SERVER 0x20
#define MAX_COUNT_POINT 30
// ************************************  TYPE DEVICE  ********************************************
#define IS_PULT 0x01
#define IS_POINT 0x10
// ********************************  COMMAND SEND CLIENTS  ***************************************
#define ARM_COMMAND_NEW_USER 0x10
#define ARM_COMMAND_LOGOUT_USER 0x12
#define ARM_COMMAND_DELETE_USER 0x11
#define CLIENT_COMMAND_REGISTRY 0x05
#define ARM_COMMAND_LAUNCH 0x01
#define ARM_COMMAND_STOP_LAUNCH 0x02
#define CLIENT_COMMAND_STATUS_POINT 0x08
// ********************************  COMMAND SEND SERVER  ***************************************
#define SERVER_COMMAND_SEND 0x05
#define SERVER_COMMAND_NEW_POINT 0x06
#define SERVER_COMMAND_DICONECT_POINT 0x07
// ************************************  EVENT_LOG ********************************************
#define START_SERVER "Запуск сервера"
#define CONNECT "Клиент подключился"
#define AUTH_CLIENT "Авторизация клиента"
#define DISCONNECT "Отключение клиента"
#define LAUNCH_SIRENA "Запуск сирены"
#define LAUNCH_SPEECH "Запуск речевого оповещения"

// ************************************  VARIABLE  **************************************

	int socket_desc, client_test, c , c_client=0, next_id=0;
	struct sockaddr_in server , client;
	struct Object *MassObj;

// ***********************************  STRUCTURE  ***************************************
struct protocolTCP
{
	uint16_t Obj; //pult or point
	uint16_t number_paket;
	uint16_t pack_lenth; 
	uint8_t id; 
	uint8_t command;
	uint8_t data[256-8];
};
struct Object
{
	int id = 0xFF;
	sockaddr_in IP_ADDR;
	int UserId = 0x00;
	bool pult=false;
	int discript;
	bool IsLaunch=false;
	char buffI[256];
	bool autorez=false;
	struct protocolTCP trnsmitter;
	struct protocolTCP *reciver;
	pthread_t *thread;
	//*** only point *** //
	int ping;
	bool AC_ON;
	uint8_t stat_line[6];
};


#endif 