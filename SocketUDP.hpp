/*
LIBRERIA SocketUDP
Contiene la classe SocketUDP, usata per gestire trasmissioni tra due terminali con l'utilizzo di socket.
La classe SocketUDP consente di inviare a e ricevere da un indirizzo, stabilito al momento della creazione
della classe, messaggi fino a 4096 caratteri
*/

#include "Address.hpp"	/* per l'utilizzo e la gestione degli indirizzi */
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>	
#define LOOPBACK_IP "127.0.0.1"		/* per mandare e ricevere messaggi sullo stesso pc */
#define LAN_IP "0.0.0.0"			/* per mandare e ricevere messaggi tra diversi pc */
#define MAX_MSG 4096		/* lunghezza massima dei messaggi */

/*
Definizione della classe SocketUDP
Contiene una variabile di tipo int sock_id, tiene traccia dell'id del socket una volta aperto
Contiene i metodi:
SocketUDP: costruttore;
~SocketUDP: distruttore;
invia: invia un messaggio tramite il socket aperto dal costruttore;
ricevi: riceve un messaggio tramite il socket aperto dal costruttore;
*/
class SocketUDP{
	private: int sock_id;
	public: SocketUDP();
		SocketUDP(int,bool);
		~SocketUDP();
		bool invia(char*,Address);
		char* ricevi(Address*);
};
/*
Costruttore della classe SocketUDP
Riceve da parametro un intero port che contiene la porta tramite la quale verranno ricevuti i messaggi
e un booleano loopback
Se loopback è impostato a false verranno trasmessi i messaggi in broadcast, se impostato a true la 
trasmissione si terrà sullo stesso terminale 
*/
SocketUDP::SocketUDP(int port =0,bool loopback=false){
	char* ip;
	struct sockaddr_in myself;
	int i;

	/* Apertura del socket */
	sock_id = socket(AF_INET,SOCK_DGRAM,0);

	/*
	Impostazione dei parametri di myself  e bind con l'indirizzo del pc corrente
	questo passaggio viene svolto solo in caso la porta passata per parametro sia diversa da 0
	*/
	if (port!=0) {

		/* Impostazione parametri di myself */
	    if(loopback)
 	      ip = strdup(LOOPBACK_IP);
            else
	      ip = strdup(LAN_IP);
      	myself.sin_family = AF_INET;
		inet_aton(ip,&myself.sin_addr);
		myself.sin_port = htons(port);
		for(i=0;i<8;i++) myself.sin_zero[i]=0;

		/* Bind del socket all'indirizzo del pc corrente */
	    bind(sock_id,(struct sockaddr*)&myself,(socklen_t)sizeof(struct sockaddr));

	}
}
/*
Distruttore della classe SocketUDP
Chiude il socket aperto con il costruttore
*/
SocketUDP::~SocketUDP(){
	close(sock_id);
}
/*
Metodo per inviare un messaggio msg ricevuto come parametro ad un indirizzo dest ricevuto come
parametro
*/
bool SocketUDP::invia(char* msg, Address dest){
   	struct sockaddr_in dest_addr;
	int len_msg;
	int len_addr;
	int ret;

	/* Definizione dei parametri utilizzati in seguito per l'invio del messaggio */
	len_msg = strlen(msg)+1;
	len_addr = sizeof(struct sockaddr_in);
	dest_addr = dest.get_address();

	/* Invio del messaggio all'indirizzo destinatario */
	ret = sendto(sock_id,msg,len_msg,0,(struct sockaddr*)&dest_addr,(socklen_t)len_addr);
        
	/* free(&dest_addr);		 ERROR: attempt to free a non-heap object */
	
	/* Restituisce true/0 se il messaggio è stato inviato correttamente, false/1 in caso di errore */
	return (ret == len_msg);
}
/*
Metodo che riceve un messaggio da un indirizzo mitt passato come parametro
*/
char* SocketUDP::ricevi(Address* mitt){
	int ret;
	char buffer[MAX_MSG+1];
	struct sockaddr_in mitt_addr;
	int len_addr;

	len_addr = sizeof(struct sockaddr_in);

	/* Ricezione del messaggio */
	ret = recvfrom(sock_id,buffer,MAX_MSG,0,(struct sockaddr*)&mitt_addr,(socklen_t*)&len_addr);
	
	/* Return in caso il messaggio ricevuto sia vuoto */
	if (ret<=0) return NULL;

	/* Aggiunta del carattere di fine riga al messaggio e return del messaggio */
	buffer[ret]='\0';
	mitt->set_address(mitt_addr);
	return strdup(buffer);
}































