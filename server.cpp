#include<stdio.h>
#include<stdlib.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h>
int socket_desc, client_test, c , read_size;
	struct sockaddr_in server , client;
	
struct data
{
	int discript;
	char messsage[1000];
};

void *init_socket(void* ssd){
	struct data *a = (struct data*)ssd; 
while((read_size = recv(a->discript , a->messsage , 1000 , 0)) > 0){
		if(a->messsage[0]=='1'){
			puts("command start sirena");
		}else if(a->messsage[0]=='1'){
			puts("command start Microphon");
		}
		printf("Descript %d ", a->discript);
		puts(a->messsage);
		write(a->discript , a->messsage , strlen((const char*)a->messsage));
	
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
	listen(socket_desc,5);
	puts("connection from an incoming client");
	
	while(1){
	struct data *client_sock = (struct data*)malloc(sizeof(struct data));
	client_sock->discript = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	//printf("DESCRIPT: %d \n",*client_sock);
	pthread_t *thread_socket = (pthread_t*)malloc(sizeof(pthread_t));
	pthread_create(thread_socket,NULL,init_socket,(int*)client_sock);
	write(client_sock->discript,"Hello from Server\n",18);
	getsockname(client_sock->discript,&getaddr,(socklen_t*)sizeof(struct sockaddr));
	printf("addr: %d",getaddr.sa_data);
	
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	puts("Connection accepted");
	}
	
	return 0;
}