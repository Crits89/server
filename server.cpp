#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h>
int socket_desc, client_test, c , c_client=0, next_id=0;
	struct sockaddr_in server , client;
	
struct data
{
	int id;
	bool pult=false;
	int discript;
	char messsage[1000];
	void* next;
};

void *init_socket(void* ssd){
	struct data *a = (struct data*)ssd;
	uint16_t size_packet=0,read_size=0;
while((read_size = recv(a->discript , a->messsage , 1000 , 0)) > 0){
		
		printf("mess: %c ", a->messsage[0]);
		printf("size: %d",(int16_t)a->messsage[1]);
		size_packet = a->messsage[1];
		
		//write(a->discript , a->messsage , strlen((const char*)a->messsage));

		if(a->messsage[0]=='5'){
			puts("this pult");
			a->pult=true;
			if(read_size-2 == a->messsage[1]){
				puts("paket OK");
			}
			printf("paket size: %d \n",read_size);
		}
		if(a->messsage[3]=='10'&& a->pult==true ){

		}
		if(a->messsage[0]=='6'){
			puts("this Point");
			a->pult=false;
			if(read_size-2 == a->messsage[1]){
				puts("paket OK");
			}
		}
		puts(a->messsage+(sizeof(char)*2));
	
}
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
}

int main(int argc , char *argv[])
{
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create soNULLcket");
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
	listen(socket_desc,100);
	puts("connection from an incoming client");
	struct data *client_sock = (struct data*)malloc(sizeof(struct data)*30);
	
	while(1){
		if(c_client>29){

		}
		 
	client_sock[next_id].discript = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	//printf("DESCRIPT: %d \n",*client_sock);
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