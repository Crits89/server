#include <stdio.h>	     //printf
#include <string.h>	     //strlen
#include <sys/socket.h>	 //socket
#include <arpa/inet.h>	 //inet_addr
#include <unistd.h>
#include <pthread.h>

bool listen_tcp=false;
bool listen_udp=false;
int sock;
char server_reply[512];


struct protocolTCP
{
	uint8_t Obj; //pult or point
	uint16_t pack_lenth; 
	uint8_t id; 
	uint8_t command;
	uint8_t data[256-7];
};
struct protocolTCP send_pac;
struct protocolTCP recive_pac;
//*****************************************CRC****************************************************
// полином 0x1163
uint16_t Crc16(uint8_t *pc, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint8_t i;

    while (len--)
    {
		printf("№%d: %#x\n",len, *pc);
        crc ^= *pc++ << 8;

        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1163 : crc << 1;
    }
    return crc;
}


void* my_listen(void *dat){
	

	while(recv(sock , &recive_pac , 256 , 0))
		{
			
			//printf("%#x\n",&prot);
			 if(memcmp(recive_pac.data,"REG OK",6)){
				 puts((char*)recive_pac.data);
			 	//puts("REGISTERED IS OK");
			 }else{
			 	puts((char*)recive_pac.data);
			  }
			memset(server_reply,0,256);
		}
		puts("socket exit\n");
	pthread_exit(0);
}


uint8_t send_packet(uint8_t command){
	
		uint16_t len = strlen((char*)send_pac.data);
		
		send_pac.pack_lenth = len+6;
		uint16_t crc = Crc16((uint8_t*)&send_pac,len+6);
		printf("crc: %d\n",crc);
		memcpy(&send_pac.data[len],&crc,2);
		if(send(sock , (char*)&send_pac , len+8 , 0) < 0)
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
	char message[256] , server_reply[256], temp[256];;
	char rejim;
	printf("%s",argv[1]);
	
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
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, my_listen, NULL);
	puts("Connected\n");
	//keep communicating with server
	

	while(1){
		send_pac.Obj = rejim;
		send_pac.id = argv[3][0];
		printf("Enter command : \n");
		scanf("%d" , &send_pac.command);
		printf("%d\n",send_pac.command);
		printf("Enter data : \n");
		scanf("%s" , send_pac.data);
		send_packet(1);
		
	}
	close(sock);
	return 0;
}