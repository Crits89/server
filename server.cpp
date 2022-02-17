#include "server.h"
#define DATA  5
#define COMMAND  3
#define ID  4
//****************command**************
#define LAUNCH 1
#define REGISTRY 5
#define IS_PULT 1
#define IS_POINT 0x10

//**************************************   CRC   *************************************************
// полином 0x1163
uint16_t Crc16(uint8_t *pc, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint8_t i;

    while (len--)
    {
		//printf("№%d: %#x\n",len, *pc);
        crc ^= *pc++ << 8;

        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1163 : crc << 1;
    }
    return crc;
}

bool send_packet(struct Object &obj,char *msg, int lenth)
{
	struct protocolTCP *prot = (struct protocolTCP*)&obj.buffI;
	
		
		prot->pack_lenth = lenth+6;
		uint16_t crc = Crc16((uint8_t*)&msg,lenth+6);
		printf("crc: %d\n",crc);
		memcpy(&prot->data[lenth],&crc,2);
		if(send(obj.discript , (char*)&prot , lenth+8 , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		return 0;
		memset(&prot,0x00,256);

}




//***************************************  LAUNCH  *************************************************
void launch(struct Object &dat){
	dat.reciver = (struct protocolTCP*)dat.buffI;
		printf("dat id: %#x\n",dat.reciver->id);

	
	for(int ind=0;ind<c_client;ind++){
		for(int dd=0;dd<dat.reciver->pack_lenth-6;dd++){
			printf("Mass: %#x\na: %#x\n",MassObj[ind].id,dat.reciver->data[dd]);
			if(dat.reciver->data[dd]==MassObj[ind].id && MassObj[ind].pult==false){
				send(MassObj[ind].discript,"launch messag\n",15,0);
				usleep(100);
			}
		}
		
		
	}
		
	
}
//***************************************  IS PULT  *************************************************
void this_pult(struct Object &dat){
	puts("this pult \n");
		dat.pult=true;
		struct protocolTCP *protI = (struct protocolTCP*)dat.buffI;
		printf("cmd: %#x\n",protI->command);
		
			if(protI->command==LAUNCH){   													// '1' оповещение
			 launch(dat);puts("launch\n");
			}
			if(protI->command==REGISTRY){ // '5' регистрация
			dat.id = dat.reciver->id;
			}	
}
//***************************************  IS POINT  *************************************************
void this_point(struct Object &dat){
	struct protocolTCP *protI = (struct protocolTCP*)dat.buffI;
	puts("this Point");
			dat.pult=false;

		if(protI->command==REGISTRY){ // '5' регистрация
			dat.id = dat.reciver->id;
			send(dat.discript,"REG OK",6,0);
			}	
}
//***************************************  SOCKET INIT  *************************************************
void *init_socket(void* ssd){
	struct Object *a = (struct Object*)ssd;
	uint16_t size_packet=0,read_size=0;
while((read_size = recv(a->discript , a->buffI , 256 , 0)) > 0){
		a->reciver = (struct protocolTCP*)a->buffI;
		uint16_t crc_pac = ((uint8_t)a->buffI[read_size-1]<<8|(uint8_t)a->buffI[read_size-2]);
		uint16_t crc_val = Crc16((uint8_t*)a->buffI,a->reciver->pack_lenth);
		for(int s=0;s<6;s++){
			printf("register addr %d: %#x\n",s,a->buffI[s]);
			}	
		
		if(crc_pac == crc_val && a->reciver->pack_lenth == read_size-2){
			puts("GOOOD");

			a->autorez = true;
			
			if(a->reciver->Obj == IS_PULT){
				this_pult(*a);
			}

			if(a->reciver->Obj == IS_POINT){
				this_point(*a);
			}
			send(a->discript,"OK",3,0);
		}else{
			send(a->discript,"CRC ERROR",10,0);
		}

			
		
		puts(a->buffI+(sizeof(char)*3));
		memset(a->buffI,0,256);
	
}
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
		next_id = a->id;
		close(a->discript);
		a->autorez=false;
		a->discript=NULL;
		a->id=0xFF;
		a->pult=false;
		memset(a->buffI,0,512);
		
		
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	
	pthread_exit(0);
}
//***************************************  MAIN  *************************************************
int main(int argc , char *argv[])
{
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	char bbaf[10];
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );
	struct sockaddr getaddr;
	//Bind
	bind(socket_desc,(sockaddr*)&server,sizeof(sockaddr));
	listen(socket_desc,128);
	puts("connection from an incoming client");
	struct Object *client_sock = (struct Object*)malloc(sizeof(struct Object)*30);
	MassObj = client_sock;
	while(1){
		if(c_client>29){

		}
		 
	client_sock[next_id].discript = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	//printf("DESCRIPT: %d \n",*client_sock);
	//client_sock[next_id].id_addres = 49;
	//client_sock[next_id].id=next_id;
	pthread_t *thread_socket = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(thread_socket,NULL,init_socket,(void*)&client_sock[next_id]);
	write(client_sock[next_id].discript,"Hello from Server\n",18);
	getsockname(client_sock->discript,&getaddr,(socklen_t*)sizeof(struct sockaddr));
	printf("addr: %d, id; %d\n",client_sock[next_id].discript,next_id);
	
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}else{
		c_client++;
		next_id++;
	}
	puts("Connection accepted");
	}	
	
	return 0;
}