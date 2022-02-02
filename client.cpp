#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	char rejim;
	printf("%s",argv[1]);
	if(argv[1]=="pult"){
		rejim='5';
		}
		if(argv[1]=="point"){
		rejim='6';
		}
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );
	char buff[1000];
	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	recv(sock,buff,1000,0);
	puts("Connected\n");
	puts(buff);
	//keep communicating with server
	while(1){
		printf("Enter message : ");
		scanf("%s" , message);
		
		//Send some data
		char temp[1000];
		
			temp[0] = '5';
		
		temp[1]=strlen(message);
		
		for(int a=0;a<strlen(message);a++){
			temp[a+2]=message[a];
		}
		printf("strlen: %d \n ",strlen(message));
		puts(temp);
		if( send(sock , temp , strlen(temp) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		
		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0)
		{
			puts("recv failed");
		}
		
		puts("Server reply :");
		puts(server_reply);
		
	}
	close(sock);
	return 0;
}