#include <stdio.h>	     //printf
#include <string.h>	     //strlen
#include <stdlib.h>
#include <sys/socket.h>	 //socket
#include <arpa/inet.h>	 //inet_addr
#include <unistd.h>
#include <pthread.h>

bool listen_tcp=false;
bool listen_udp=false;
uint16_t number_packet = 1;
int sock;
uint8_t buffer[256];
struct protocolTCP
{
	uint16_t Obj; //pult or point
	uint16_t number_paket;
	uint16_t pack_lenth; 
	uint8_t id; 
	uint8_t command;
	uint8_t data[256-9];
};
struct protocolTCP send_pac;
struct protocolTCP recive_pac;
//***************************************  CRC  **************************************************
// полином 0x1163
uint16_t Crc16(uint8_t *pc, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint8_t i;

    while (len--)
    {
        crc ^= *pc++ << 8;

        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1163 : crc << 1;
    }
    return crc;
}

void* play(void *d){
				system("nc -l -u 4424 | mpg123 -");
				
}
//****************************************  Play  ***************************************************
void* my_listen(void *dat){
	
ssize_t read_size;

	while(read_size = recv(sock , &buffer , 256 , 0))
		{
		recive_pac = *(struct protocolTCP*)&buffer;
		uint16_t crc_pac = (uint16_t)(buffer[read_size-1]<<8 | buffer[read_size-2]);
		
		uint16_t crc_val = Crc16((uint8_t*)buffer,recive_pac.pack_lenth);

		printf("crc in: %#x\ncrc val: %#x\n",crc_pac,crc_val);
		pthread_t pthr;
			if(recive_pac.command==1){
				pthread_create(&pthr,NULL,play,NULL);
				puts("START");
			}
			
			memset(&buffer,0,256);
		}
		puts("socket exit\n");
	pthread_exit(0);
	
}
uint8_t send_packet(uint8_t command){
	
		uint16_t len = strlen((char*)send_pac.data);
		
		send_pac.pack_lenth = len+8;
		uint16_t crc = Crc16((uint8_t*)&send_pac,len+8);
		printf("crc send: %#x\n",crc);
		send_pac.number_paket = number_packet;
		memcpy(&send_pac.data[len],&crc,2);
		if(send(sock , (char*)&send_pac , len+10 , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		return 0;
		memset(&send_pac,0x00,256);
}
int main(int argc , char *argv[])
{
	struct sockaddr_in server;
	char message[256];
	char rejim;
	//printf("%s",argv[1]);

	if(strcmp(argv[2],"pu")==0){
		puts("OK");
		rejim=0x01;
		}
		if(strcmp(argv[2],"po")==0){
		rejim=0x10;
		}
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, my_listen, NULL);
	puts("Connected\n");
	
	

	while(1){
		send_pac.Obj = rejim;
		send_pac.id = argv[3][0];
		send_pac.number_paket = number_packet;
		printf("Enter command : \n");
		scanf("%d" , &send_pac.command);
		printf("%d\n",send_pac.command);
		printf("Enter data : \n");
		scanf("%s" , send_pac.data);
		send_packet(1);
		number_packet++;
	}
	close(sock);
	return 0;
}