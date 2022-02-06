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
void* my_listen(void *dat){
	puts("in my_listen\n");

	while(recv(sock , server_reply , 512 , 0))
		{
			puts(server_reply);
		}
		puts("socket exit\n");
	pthread_exit(0);
}


int main(int argc , char *argv[])
{
	
	struct sockaddr_in server;
	char message[500] , server_reply[500], temp[500];;
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
		printf("Enter message : \n");
		
		scanf("%s" , &message[4]);
		
		message[0] = rejim;
		message[1] = (unsigned)0xFF;
		message[2] = strlen(&message[3]);
		message[3] = argv[3][0];
		
		puts(message);
		printf("1: %d\n2: %d\n3: %d\n4: %d\n",message[0],message[1],message[2],message[3]);
		
		if( send(sock , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
	}
	close(sock);
	return 0;
}