#include "server.h"



void launch(struct Object &dat){

	uint8_t count=0;
	for(int i=0;i<c_client+1;i++){
		if(dat.buffI[i+5]=='0'){
			printf("break");
		break;
	}else{
		count++;
		printf("i: %d\n",count);
	}
	}
	for(int a=0;a<count;a++){
	for(int i=0;i<c_client;i++){
		if(dat.buffI[a+5]==MassObj[i].id_addres){
	 printf("buf: %d\n",dat.buffI[a+5]);
	 printf("discript: %d\nautorez: %d\naddr: %d\npult: %d\n",MassObj[i].discript,MassObj[i].autorez,MassObj[i].id_addres,MassObj[i].pult);
			send(MassObj[i].discript,"start opv",9,0);
			usleep(100);
		}
	}
	}
	
}

void this_pult(struct Object &dat){
	puts("this pult \n");
		dat.pult=true;
		struct protocolTCP *protI = (struct protocolTCP*)dat.buffI;
			if(dat.buffI[4]==49){   													// '1' оповезение
			 launch(dat);puts("launch\n");
			}
			if(dat.buffI[4]==53){dat.id_addres = dat.buffI[3];puts("register addr\n");	// '5' регистрация
			uint8_t buf[15] = "registr addr: ";
			buf[14] = dat.id_addres;
			send(dat.discript,&buf,15,0);
			}	
}

void this_point(struct Object &dat){
	puts("this Point");
			dat.pult=false;
		struct protocolTCP *protI = (struct protocolTCP*)dat.buffI;
		if(dat.buffI[3]==53){dat.id_addres = dat.buffI[3];puts("register addr\n");
			uint8_t buf[15] = "registr addr: ";
			buf[14] = dat.id_addres;
			send(dat.discript,&buf,15,0);
			}
}



void *init_socket(void* ssd){
	struct Object *a = (struct Object*)ssd;
	uint16_t size_packet=0,read_size=0;
while((read_size = recv(a->discript , a->buffI , 512 , 0)) > 0){
		if((unsigned char)a->buffI[1]==255){
			a->buffI[1]=0;
			
		}
		
		size_packet = (uint16_t)(a->buffI[2]|a->buffI[1]<<8);
		printf("1: %d \n2: %d\n",(unsigned char)a->buffI[1]<<8,(unsigned char)a->buffI[2]);
		printf("%d : %d\n",read_size-3,size_packet);
		if(read_size-3 == size_packet){
			a->autorez = true;
			
			if(a->buffI[0]==0x01){
				this_pult(*a);
			}

			if(a->buffI[0]==0x10){
				this_point(*a);
			}
		}else{

		}

			printf("paket size: %d \n",read_size);
		
		puts(a->buffI+(sizeof(char)*3));
		memset(a->buffI,0,512);
	
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
		memset(a->buffO,0,512);
		
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
	
	pthread_exit(0);
}

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
	client_sock[next_id].id=next_id;
	pthread_t *thread_socket = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(thread_socket,NULL,init_socket,(void*)&client_sock[next_id]);
	write(client_sock[next_id].discript,"Hello from Server\n",18);
	getsockname(client_sock->discript,&getaddr,(socklen_t*)sizeof(struct sockaddr));
	printf("addr: %d, id; %d",client_sock[next_id].discript,next_id);
	
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