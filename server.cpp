#include "server.h"
uint16_t number_packet = 5;

//**************************************   CRC   *************************************************
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
		// printf("CRC: %d\n",crc);
	}
	printf("CRC: %d\n", crc);
	return crc;
}

//**************************************   send_packet   *************************************************

bool send_packet(struct Object &obj, uint8_t cmd, char *msg, int lenth)
{
	struct protocolTCP prot;
	memset(&prot, 0x00, 256);
	memcpy(prot.data, msg, lenth);
	prot.pack_lenth = lenth + 8;
	prot.number_paket = number_packet;
	number_packet++;
	prot.id = SERVER_ID;
	prot.command = cmd;
	prot.Obj = OBJECT_SERVER;
	uint16_t crc = Crc16((uint8_t *)&prot, lenth + 8);
	memcpy(&prot.data[lenth], &crc, 2);
	printf("crc out: %#x\n", crc);
	if (send(obj.discript, (uint8_t *)&prot, lenth + 10, 0) < 0)
	{
		puts("Send failed");
		return 1;
	}
	return 0;
}
//***************************************  RESENDING UDP  *******************************************

void *play(void *d)
{
	pthread_exit(0);
}

//***************************************  LAUNCH  *************************************************

void launch(struct Object &dat)
{
	dat.reciver = (struct protocolTCP *)dat.buffI;
	for (int ind = 0; ind < c_client; ind++)
	{
		printf("ID_LAUNCH: %d", MassObj[ind].id);
		for (int dd = 0; dd < dat.reciver->pack_lenth - 8; dd++)
		{
			if (dat.reciver->data[dd] == MassObj[ind].id && MassObj[ind].pult == false)
			{
				send_packet(MassObj[ind], 1, "launch\x00", 7);
				usleep(1000);
				printf("IP_LAUNCH: %s\n", inet_ntoa(MassObj[ind].IP_ADDR.sin_addr));
				std::string start = "nc -l -u 2301 | nc -u ";
				start.append(inet_ntoa(MassObj[ind].IP_ADDR.sin_addr));
				start.append(" 4424\x00");
				std::cout << start;
				system(start.c_str());
				MassObj[ind].IsLaunch = true;
			}
		}
	}
}

//***************************************  IS PULT  *************************************************

void this_pult(struct Object &dat)
{
	puts("this pult \n");
	printf("command: %#x\n",dat.reciver->command);
	printf("IP: %s\n", inet_ntoa(dat.IP_ADDR.sin_addr));
	bool auth = false;
	dat.pult = true;

	if (dat.reciver->command == ARM_COMMAND_LAUNCH) 		// 0x01 оповещение
	{ 
		if(dat.autorez == true)
		{
			launch(dat);
		}
	}
	if (dat.reciver->command == CLIENT_COMMAND_REGISTRY) 	// 0x05 регистрация
	{ 
		char *qlog;
		char login[64];
		char pass[64];
		for (int dd = 0; dd < MAX_COUNT_POINT; dd++)
		{
			if (MassObj[dd].id == dat.reciver->id)
			{
				auth = true;
				close(dat.discript);
				delete dat.thread;
			}
		}

		if (auth == false)
		{

			int len = dat.reciver->pack_lenth - 10;
			if (len >= 5)
			{
				int offset = sprintf(login, (char *)&dat.reciver->data);
				sprintf(pass, (char *)&dat.reciver->data[offset + 1]);
				qlog = SQL_USER_QUERY(login, 2);
				if (strcmp(pass, qlog) == 0)
				{
					dat.UserId = atoi(SQL_USER_QUERY(login, 0));
					SQL_USER_TIME(dat.UserId,true);
					dat.reciver->data;
					dat.id = dat.reciver->id;
					dat.autorez = true;
					SQL_QUERY_SET(0,0,inet_ntoa(dat.IP_ADDR.sin_addr),"АРМ","Успешно",0,"Регистрация",0,0,0);
					usleep(1000);
					if (strcmp(login, "Admin") == 0)
					{
						send_packet(dat, SERVER_COMMAND_SEND, "REG_OKY\0x00", 8);
					}
					else
					{
						send_packet(dat, SERVER_COMMAND_SEND, "REG_OK\0x00", 7);
					}
					usleep(1000);
					for (int dd = 0; dd < MAX_COUNT_POINT; dd++)
					{
						if (MassObj[dd].pult == false && MassObj[dd].id != 0x00 && MassObj[dd].autorez == true)
						{
							char ss[2];
							ss[0] = MassObj[dd].id;
							ss[1] = 0x00;
							send_packet(dat, SERVER_COMMAND_NEW_POINT, (char *)ss, 1);
							usleep(10000);
						}
					}
				}
				else
				{
					send_packet(dat, SERVER_COMMAND_SEND, "REG_ERR\0x00", 8);
				}
			}
		}
		printf("ID: %d\nlogin: %s\npass: %s\n", dat.UserId, login, pass);
		delete qlog;
	}
	if (dat.reciver->command == ARM_COMMAND_STOP_LAUNCH)    // 0x02 остановка оповещения
	{
		for (int a = 0; a < MAX_COUNT_POINT; a++)
			if (MassObj[a].discript != 0 && MassObj[a].IsLaunch == true)
			{
				MassObj[a].IsLaunch = false;
				send_packet(MassObj[a], ARM_COMMAND_STOP_LAUNCH, "STOP\x00", 6);
				usleep(1000);
				printf("STOP\n");
			}
	}
	if (dat.reciver->command == ARM_COMMAND_NEW_USER) 		// 0x10 Добавление нового пользователя
	{
		if(dat.UserId == 1 && dat.autorez==true){
			char login[64];
			char pass[64];
			char longname[256];
			if(dat.reciver->pack_lenth > 20){
				
				int off1 = sprintf(login,(char*)&dat.reciver->data);
				printf("LOG: %s\n",login);
				int off2 = sprintf(pass,(char*)&dat.reciver->data[off1+1]);
				printf("Pass: %s\n",pass);
				sprintf(longname,(char*)&dat.reciver->data[off2 + off1 + 2]);
				printf("name: %s\n",longname);
				if(strcmp(SQL_USER_QUERY(login,1),login)!=0)
				{
					if(SQL_ADD_NEW_USER(login,pass,longname))
					{
						send_packet(dat, ARM_COMMAND_NEW_USER, "Nuok\x00", 6);
					}
				}
				else
				{
					send_packet(dat, ARM_COMMAND_NEW_USER, "Nuer\x00", 6);
				}
			}
		}
	}
	if (dat.reciver->command == ARM_COMMAND_DELETE_USER)	// 0x11 Удаление пользователя
	{
		if(dat.UserId == 1 && dat.autorez==true)
		{
			char login[64];
			char pass[64];
			bool deleted=false;
			if(dat.reciver->pack_lenth > 10)
			{
				int off1 = sprintf(login,(char*)&dat.reciver->data);
				int off2 = sprintf(pass,(char*)&dat.reciver->data[off1+1]);
				if(strcmp(SQL_USER_QUERY(login,1),login)==0)
				{
					if(strcmp(SQL_USER_QUERY(login,2),pass)==0)
					{
						if(SQL_USER_DELETE(SQL_USER_QUERY(login,0)))
						{
							if(strcmp(login,"Admin")!=0)
							{
								send_packet(dat, ARM_COMMAND_DELETE_USER, "Nuok\x00", 6);
								deleted=true;
							}
						}

					}
				}
				if(deleted==false)
				{
					send_packet(dat, ARM_COMMAND_DELETE_USER, "Nuer\x00", 6);
				}
			}
		}
	}

}
//***************************************  IS POINT  *************************************************

void this_point(struct Object &dat)
{
	puts("this Point");
	dat.pult = false;
	bool auth = false;
	if (dat.reciver->command == CLIENT_COMMAND_REGISTRY) // '5' регистрация
	{ 
		for (int dd = 0; dd < MAX_COUNT_POINT; dd++)
		{
			if (MassObj[dd].id == dat.reciver->id)
			{
				auth = true;
				
			}
		}
		if (auth == false)
		{
			
			
			dat.id = dat.reciver->id;
			dat.autorez = true;
			send_packet(dat, SERVER_COMMAND_SEND, "REG_OK", 6);
			usleep(100);
			for (int b = 0; b < MAX_COUNT_POINT; b++)
			{
				if (MassObj[b].pult == true && MassObj[b].autorez == true)
				{
					char ss[2];
					ss[0] = dat.id;
					ss[1] = 0x00;
					send_packet(MassObj[b], SERVER_COMMAND_NEW_POINT, (char *)ss, 1);
				}
			}
			SQL_QUERY_SET(0,0,inet_ntoa(dat.IP_ADDR.sin_addr),"Приемный модуль","Успешно",0,"Регистрация",0,0,0);
		}
	}
	if (dat.reciver->command == CLIENT_COMMAND_STATUS_POINT) // '8' состояние объекта
	{ 
		dat.AC_ON = dat.reciver->data[0];
		memset(dat.stat_line, 0x00, 6);
		memcpy(dat.stat_line, &dat.reciver->data[0], 6);
		fprintf(stdout, "command 8 ok data: %s", dat.stat_line);
		for (int c = 0; c < MAX_COUNT_POINT; c++)
		{
			if (MassObj[c].id != 0x00 && MassObj[c].pult == true)
			{
				send_packet(MassObj[c], CLIENT_COMMAND_STATUS_POINT, (char *)&dat.AC_ON, 6);
			}
		}
	}
}

//***************************************  SOCKET INIT  *************************************************

void *init_socket(void *ssd)
{
	struct Object *a = (struct Object *)ssd;
	uint16_t size_packet = 0, read_size = 0;
	while ((read_size = recv(a->discript, a->buffI, 256, 0)) > 0)
	{
		a->reciver = (struct protocolTCP *)a->buffI;
		uint16_t crc_pac = ((uint8_t)a->buffI[read_size - 1] << 8 | (uint8_t)a->buffI[read_size - 2]);
		uint16_t crc_val = Crc16((uint8_t *)a->buffI, a->reciver->pack_lenth);
		// printf("crc_val: %#x\ncrc_pac: %#x\n",crc_val, crc_pac);
		if (crc_pac == crc_val && a->reciver->pack_lenth == read_size - 2)
		{
			puts("GOOD");
			if (a->reciver->Obj == IS_PULT)
			{
				this_pult(*a);
			}

			if (a->reciver->Obj == IS_POINT)
			{
				this_point(*a);
			}
		}
		else
		{
			send_packet(*a, SERVER_COMMAND_SEND, "ERROR", 6);
		}

		puts(a->buffI + (sizeof(char) * 3));
		memset(a->buffI, 0, 256);
	}
	if (read_size == 0)
	{
		if (a->pult == false)
		{
			for (int d = 0; d < MAX_COUNT_POINT; d++)
			{
				if (MassObj[d].pult == true)
				{
					char ss[2];
					ss[0] = a->id;
					ss[1] = 0x00;
					send_packet(MassObj[d], SERVER_COMMAND_DICONECT_POINT, (char *)a, 1);
				}
			}
		}
		else
		{
			SQL_USER_TIME(a->UserId,false);
		}
		puts("Client disconnected");
		fflush(stdout);
		close(a->discript);
		a->autorez = false;
		a->discript = NULL;
		a->reciver = 0x00;
		a->id = 0x00;
		a->pult = false;
		memset(a->buffI, 0, 256);
		c_client--;
	}
	else if (read_size == -1)
	{
		perror("recv failed");
	}
	pthread_exit(0);
}
//***************************************  MAIN  *************************************************
int main(int argc, char *argv[])
{
	setlocale(LC_ALL, "Russian");
	// Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	char bbaf[10];
	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);
	struct sockaddr getaddr;
	// Bind
	bind(socket_desc, (sockaddr *)&server, sizeof(sockaddr));
	listen(socket_desc, 128);
	SQL_DB_INIT();
	SQL_QUERY_SET(0,0,0,"Сервер","Успешно",0,"Запуск",0,0,0);
	puts("connection from an incoming client");
	struct Object *client_sock = (struct Object *)malloc(sizeof(struct Object) * MAX_COUNT_POINT);
	memset(client_sock, 0, sizeof(struct Object) * MAX_COUNT_POINT);
	MassObj = client_sock;
	next_id = 0;
	while (1)
	{
		if (c_client > 29)
		{
		}
		// printf("next_id: %d\n",next_id);
		client_sock[next_id].discript = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
		client_sock[next_id].IP_ADDR.sin_addr = client.sin_addr;
		if (strcmp(inet_ntoa(client.sin_addr), "0.0.0.0") == 0)
		{
			close(client_sock[next_id].discript);
			client_sock[next_id].discript = 0;
		}
		else
		{
			pthread_t *thread_socket = (pthread_t *)malloc(sizeof(pthread_t));
			pthread_create(thread_socket, NULL, init_socket, (void *)&client_sock[next_id]);
			send_packet(client_sock[next_id], SERVER_COMMAND_SEND, "connected", 9);
			getsockname(client_sock->discript, &getaddr, (socklen_t *)sizeof(struct sockaddr));
			printf("IP_ADDRES: %s\n", inet_ntoa(client.sin_addr));
			client_sock->thread = thread_socket;
			if (client_sock < 0)
			{
				perror("accept failed");
				return 1;
			}
			else
			{
				c_client++;
				next_id++;
			}
			for (int b = 0; b < MAX_COUNT_POINT; b++)
			{
				if (client_sock[b].discript == 0)
				{
					next_id = b;
					break;
				}
			}
			puts("Connection accepted");
		}
	}
	return 0;
}