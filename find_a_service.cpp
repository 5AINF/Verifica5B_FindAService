#include <sqlite3.h>
#include "SocketUDP.hpp"

#define REQUEST "FS0.1RQST["	//Usato per il controllo della richiesta corretta
#define RESPONSE "FS0.1RSPN"	//Usato per costruire la risposta
#define INVALID_REQUEST "FS0.1RSPNKO"	//Codice da inviare in caso di richiesta errata
#define MAX_BUFFER 4096		//Per la costruzione dei buffer
#define DBNAME "DBServizi"	//Nome del database
#define QUERY "SELECT IPAddress, port FROM Servizi WHERE IDservizio = \""	//Struttura della query

//Funzione di callback, prende IPAddress e port dal risultato della query e costruisce un pezzo della risposta
int callback(void* s, int count, char** data, char** columns) {
	char* fetch = (char*)s;
	sprintf(fetch,"[%s][%s]",data[0],data[1]);	//Secondo pezzo della risposta per il client
	return 0;
}

int main(int argc, char* argv[]) {
	int port;
	int sock_id;
	Address sender;
	
	if(argc!=2) {
		printf("USAGE:%s PORT",argv[0]);
		return -1;
	}
	
	port = atoi(argv[1]);
	SocketUDP myself(port,false);

	char* request = myself.ricevi(&sender);
	if(strstr(request,REQUEST)==NULL) {	//Invia il codice di errore al client in caso di richiesta errata
		myself.invia(INVALID_REQUEST,sender);
		return -2;
	}

	char* id_start = request + strlen(REQUEST);	//Si sposta all'inizio dell'ID del servizio nella richiesta
	char buffer[MAX_BUFFER];
	int i;
	for(i=0;id_start[i]!=']'&&id_start[i]!='\0';i++) {		//Preleva l'ID del servizio dalla richiesta
		buffer[i] = id_start[i];
	}
	if(id_start[i]=='\0') {		//Secondo controllo errori, controlla se la parentesi è stata chiusa dopo l'ID del servizio
		myself.invia(INVALID_REQUEST,sender);
		return -2;
	}
	buffer[i] = '\0';

	//Connessione al database
	sqlite3* sql_conn;
	sqlite3_open(DBNAME,&sql_conn);
	char query[MAX_BUFFER];
	sprintf(query,"%s%s\"",QUERY,buffer);	//Costruisce la query
	
	char fetch[MAX_BUFFER];
	sqlite3_exec(sql_conn,query,callback,fetch,NULL);	//Esegue la query e mette il risultato dentro fetch[]
	
	char response[MAX_BUFFER];
	sprintf(response,"%s%s",RESPONSE,fetch);	//Costruisce la risposta per il client
	
	myself.invia(response,sender);	//Invia la risposta al client
	
	return 0;
}
