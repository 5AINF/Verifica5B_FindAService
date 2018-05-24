#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <sqlite3.h>

#define REQUEST "FS0.1RQST["
#define RESPONSE "FS0.1RSPN"
#define INVALID_REQUEST "FS0.1RSPNKO"
#define MAX_BUFFER 4096
#define DBNAME "DBServizi"
#define QUERY "SELECT IPAddress, port FROM Servizi WHERE IDservizio = \""

int callback(void* s, int count, char** data, char** columns) {
	char* fetch = (char*)s;
	sprintf(fetch,"[%s][%s]",data[0],data[1]);
	return 0;
}

int main(int argc, char* argv[]) {
	int port;
	int sock_id;
	Address* sender;
	
	if(argc!=2) {
		printf("USAGE:%s PORT",argv[0]);
		return -1;
	}
	
	port = atoi(argv[1]);
	SocketUDP myself(port);
	
	char* request = myself.ricevi(sender);
	if(strstr(request,REQUEST)==NULL) {
		myself.invia(*sender,INVALID_REQUEST);
		delete(sender);
		return -2;
	}
	
	char* id_start = request + strlen(REQUEST);
	char buffer[MAX_BUFFER];
	int i;
	for(i=0;id_start[i]!=']';i++) {
		buffer[i] = id_start[i];
	}
	buffer[i] = '\0';
	
	sqlite3* sql_conn;
	sqlite3_open(DBNAME,&sqlconn);
	char query[MAX_BUFFER];
	sprintf(query,"%s%s\"",QUERY,buffer);
	
	char fetch[MAX_BUFFER];
	sqlite3_exec(sql_conn,query,callback,fetch,NULL);
	
	char response[MAX_BUFFER];
	sprintf(response,"%s%s",RESPONSE,fetch);
	
	myself.invia[*sender,response];
	
	delete(sender);
	
	return 0;
}
