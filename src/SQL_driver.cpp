#include <time.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //strlen
#include <sys/stat.h>
#include <unistd.h> //write
#include <iostream>
#define USERS Users.db

char DB_name[40];
char LogFileDir[40];
sqlite3 *DB;

sqlite3_stmt *SqlResult;

//***************************************  SQL_DB_INIT  *************************************************
void SQL_DB_INIT()
{
	time_t times = time(NULL); // Current time
	struct tm *timeStr = localtime(&times);
	const char *SQL = "CREATE TABLE IF NOT EXISTS LOG_LAUNCH(Date string, Time string, Mode string, Object string, IP string, Result string); CREATE TABLE IF NOT EXISTS LOG_INFO(Event string, Date string, Time string, Object string, IP string, Result string);"; // INSERT INTO LOG VALUES(1,2,3); INSERT INTO LOG SELECT * FROM LOG;";
	const char *SQL_User = "CREATE TABLE IF NOT EXISTS USER(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, login string, pass string, name string, afterlogIn string, afterlogOut string);";
	struct stat st = {0};
	char isFolder[128] = {0};
	if (stat("./LOG", &st) == -1)
	{
		mkdir("LOG", 0700);
	}

	sprintf(isFolder, "./LOG/%d", timeStr->tm_year + 1900);
	if (stat(isFolder, &st) == -1)
	{
		mkdir(isFolder, 0700);
	}
	sprintf(DB_name, "%d-%d", timeStr->tm_mon + 1, timeStr->tm_year + 1900);

	char *err = 0;
	FILE *DB_FILE;
	sprintf(LogFileDir, "%s/%s", isFolder, DB_name);
	DB_FILE = fopen(LogFileDir, "r");
	if (DB_FILE)
	{
		printf("File exists\n");
		sqlite3_open(LogFileDir, &DB);
	}
	else
	{
		printf("File not exists\n");
		DB_FILE = fopen(LogFileDir, "w");
		fclose(DB_FILE);
		sqlite3_open(LogFileDir, &DB);
		sqlite3_exec(DB, SQL, 0, 0, 0);
	}
	FILE *DBF = fopen("Users.db", "r");
	if (!DBF)
	{
		// fclose(DBF);
		sqlite3 *DBuser;
		DBF = fopen("Users.db", "w");
		fclose(DBF);
		sqlite3_open("Users.db", &DBuser);
		sqlite3_exec(DBuser, SQL_User, 0, 0, 0);
		sqlite3_exec(DBuser, "INSERT INTO USER(login, pass, name, afterlogIn, afterlogOut) VALUES(\"Admin\",\"Admin\",\"Administrator\",\"0:0:0\",\"0\");", NULL, 0, 0);
		sqlite3_close(DBuser);
	}
	sqlite3_close(DB);
	
}
//************************************  SQL_QUERY  **********************************************

bool SQL_QUERY_SET(char index_table, int UserId, char *IP, char *Object, char *Result, char *Mode, char *Event, char *login, char *pass, char *name) // index_tab = 0 (LOG_INFO) index_tab = 1 (LOG_LAUNCH)
{
	time_t times = time(NULL); // Current time
	struct tm *timeStr = localtime(&times);
	char date[64];
	char time[64];
	sprintf(date, "%d-%d-%d",timeStr->tm_year + 1900,timeStr->tm_mon + 1,  timeStr->tm_mday);
	sprintf(time, "%d:%d:%d", timeStr->tm_hour, timeStr->tm_min, timeStr->tm_sec);
	const char *SQL = "CREATE TABLE IF NOT EXISTS LOG_LAUNCH(UserId int, Date string, Time string, Mode string, Object string, IP string, Result string); CREATE TABLE IF NOT EXISTS LOG_INFO(Event string, Date string, Time string, Object string, IP string, Result string);"; // INSERT INTO LOG VALUES(1,2,3); INSERT INTO LOG SELECT * FROM LOG;";
	char isFolders[64] = {0};
	struct stat st = {0};
	if (stat("./LOG", &st) == -1)
	{
		mkdir("LOG", 0700);
	}

	sprintf(isFolders, "./LOG/%d", timeStr->tm_year + 1900);
	if (stat(isFolders, &st) == -1)
	{
		mkdir(isFolders, 0700);
	}

	sprintf(DB_name, "%d-%d", timeStr->tm_mon + 1, timeStr->tm_year + 1900);
	char *err = 0;
	FILE *DB_FILE;
	sprintf(LogFileDir, "%s/%s", isFolders, DB_name);
	DB_FILE = fopen(LogFileDir, "r");
	if (DB_FILE)
	{
		printf("File exists\n");
		sqlite3_open(LogFileDir, &DB);
	}
	else
	{
		printf("File not exists\n");
		DB_FILE = fopen(LogFileDir, "w");
		sqlite3_open(LogFileDir, &DB);
		sqlite3_exec(DB, SQL, 0, 0, &err);
	}
	char query_temp[512] = {0};
	switch (index_table)
	{
	case 0:
		sprintf(query_temp, "INSERT INTO LOG_INFO (Date, Time, Event, Object, IP, Result) VALUES (\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", date, time, Event, Object, IP, Result);

		if (sqlite3_exec(DB, query_temp, 0, 0, &err))
		{

			fprintf(stderr, "Error SQL: %sn", err);
			sqlite3_free(err);
			return 1;
		}
		break;
	case 1:
		sprintf(query_temp, "INSERT INTO LOG_LAUNCH(UserId, Date, Time, Mode, Object, IP, Result) VALUES(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");", UserId, date, time, Mode, Object, IP, Result);
		if (sqlite3_exec(DB, query_temp, 0, 0, &err))
		{

			fprintf(stderr, "Error SQL: %s\n", err);
			sqlite3_free(err);
			return 1;
		}
		break;
	
	default:
		return 1;
	}
	sqlite3_close(DB);
	printf("TOCHKA 4");
	return 0;
}

//************************************  SQL_QUERY_GET  **********************************************

bool SQL_QUERY_GET(char *SQL_DB, char *Query) // index_tab = 0 (LOG_INFO) index_tab = 1 (LOG_LAUNCH)
{
	int s = 0;
	sqlite3_open(SQL_DB, &DB);
	int rc = sqlite3_prepare(DB, Query, -1, &SqlResult, NULL);
	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Error SQL get query\n");
		sqlite3_close(DB);
		return 1;
	}
	
	sqlite3_close(DB);
	return 0;
}
//************************************  QUERY_USER  **********************************************
char *SQL_USER_QUERY(char *Login, int column)
{
	sqlite3 *DB_user;
	char query[256];
	char *rets = new char[64];
	sqlite3_open("Users.db", &DB_user);
	sprintf(query, "SELECT * FROM USER WHERE login=\"%s\";", Login);
	int ret = sqlite3_prepare(DB_user, query, -1, &SqlResult, NULL);

	if (ret != SQLITE_OK)
	{
		fprintf(stderr, "Error SQL get query\n");
		sqlite3_close(DB_user);
		return rets;
	}
	while (sqlite3_step(SqlResult) == SQLITE_ROW)
	{
		sprintf(rets, (char *)sqlite3_column_text(SqlResult, column));
	}
	sqlite3_close(DB_user);
	return rets;
}

//************************************  UPDATE_USER_TIME  **********************************************
void SQL_USER_TIME(int UserId, bool LogIn)
{
	
	time_t times = time(NULL); // Current time
	struct tm *timeStr = localtime(&times);
	char date[64];
	char time[64];
	sprintf(date, "%d-%d-%d",timeStr->tm_year + 1900,timeStr->tm_mon + 1,  timeStr->tm_mday);
	sprintf(time, "%d:%d:%d", timeStr->tm_hour, timeStr->tm_min, timeStr->tm_sec);
	sqlite3 *DB_user;
	char query[256];
	sqlite3_open("Users.db", &DB_user);
	if(LogIn==true)
	{
    	sprintf(query,"UPDATE USER set afterlogIn = \"%s %s\" where id = %d; ",date, time , UserId);
	}
	else
	{
		sprintf(query,"UPDATE USER set afterlogOut = \"%s %s\" where id = %d; ",date, time , UserId);
	}
	sqlite3_exec(DB_user, query, 0, 0, 0);
	sqlite3_close(DB_user);
}
//************************************  ADD_NEW_USER  **********************************************
bool SQL_ADD_NEW_USER(char *Login,char *pass, char *name)
{
	time_t times = time(NULL); // Current time
	struct tm *timeStr = localtime(&times);
	char date[64];
	char time[64];
	sprintf(date, "%d-%d-%d",timeStr->tm_year + 1900,timeStr->tm_mon + 1,  timeStr->tm_mday);
	sprintf(time, "%d:%d:%d", timeStr->tm_hour, timeStr->tm_min, timeStr->tm_sec);
	char query_temp[256]; 
	sqlite3 *DB_user;
	sqlite3_open("Users.db", &DB_user);
		sprintf(query_temp, "INSERT INTO USER(login, pass, name, afterlogIn) VALUES(\"%s\",\"%s\",\"%s\",\"%s %s\");", Login, pass, name, date, time);
		if (sqlite3_exec(DB_user, query_temp, 0, 0, 0))
		{
			sqlite3_close(DB_user);
			return 0;
		}
		sqlite3_close(DB_user);
	return 1;
}
//************************************  SQL_USER_DELETE  **********************************************
bool SQL_USER_DELETE(char *UserId)
{
	char query_temp[256]; 
	sqlite3 *DB_user;
	sqlite3_open("Users.db", &DB_user);
		sprintf(query_temp, "DELETE from USER where id = %s;", UserId);
		if (sqlite3_exec(DB_user, query_temp, 0, 0, 0))
		{
			sqlite3_close(DB_user);
			return 0;
		}
		sqlite3_close(DB_user);
	return 1;
}
void test()
{
	char query_temp[256]; 
	sqlite3 *DB_user;
	sqlite3_open("Users.db", &DB_user);
		//sprintf(query_temp, "SELECT * FROM USER WHERE afterlogIn BETWEEN '2022-4-25 17:00:00.000' AND '2022-4-27 08:30:00.000';", UserId);
		if (sqlite3_exec(DB_user, "DELETE FROM USER WHERE afterlogIn BETWEEN \"2022-4-26 15:40:31\" AND \"2022-4-26 15:41:33\";", 0, 0, 0))
		{
			sqlite3_close(DB_user);
		}
		sqlite3_close(DB_user);
	
	
}