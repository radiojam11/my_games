#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>


// *************************** DECLARING FUNCTIONS **********************************

void error(const char* msg);          // LOG Errors
void handle_client(int client_fd);    // HANDLE Client connections
void handle_signal(int signal);       // HANDLE Signal SIGINT
void handle_exit();                   // HANDLE Exit cleanly (depend on child_count)

int exit_flag=0;
int child_count=0;


// ********************************************************************************

int main(void){

 int passive_fd,                  // a passive socket will be binded the listening port
 client_fd;                       // an active socket will be spawned for each incoming connection.

 struct sockaddr_in serv_data;    // the type of connection (INET/UNIX) 
                                  // the accepted ip range
								  // and the port on wich we will listen are stored in this struct.
 


 // Lets instantiate the socket file descriptor.
 // socket ( NET DOMAIN , TYPE (TCP:STREAM/UDP:DGRAM) , PROTOCOL )
 // PROTOCOL is only defined when there is any additional protocol

 if ( 0 > ( passive_fd = socket(AF_INET,SOCK_STREAM,0) ) )
   error ("ERROR could not create socket"); 
 
 int enable=1;
 setsockopt(passive_fd,           // Takes socket as first argoument
            SOL_SOCKET,           // level as second. this is the protocol level at wich the opt resides
			                      // this is the SOcket Level, if e.g. an option need to be interpreted 
								  // by the TCP the right chose will be IPPROTO_TCP.
			SO_REUSEADDR,         // This will enable the reuse of the socket even if it is in TIME_WAIT
			                      // that is the delay used to ensure that the other peer receives ACK
			&enable,              // New option value, 1 as enabled.
			sizeof(enable));      // Size of new option. 
                                  // If you want to add other options such as SO_KEEPALIVE add it as a logic or. 

 // Initialize data structure and fill struct with metadata

 memset(&serv_data, '0', sizeof(serv_data  ));

 serv_data.sin_family       = AF_INET;       
 serv_data.sin_addr.s_addr  = INADDR_ANY; 
 serv_data.sin_port         = htons(9999);


 // Bind passive socket to the host:port pair defined in the struct

 if ( 0 > bind( passive_fd, (struct sockaddr*) &serv_data, sizeof(serv_data))) 
   error("ERROR binding socket");

 // Let the socket be able to listen for incoming client connections
 
 if ( 0 > listen( passive_fd, 5 )) 
   error("ERROR starting to listen");

 // Define the sigint signal handler

 signal(SIGINT, handle_signal);

 // Lets spawn an active socket for each incoming connection and fork

 for(;;){
   
   if (exit_flag){                                   // Handle SIGINT and wait for childs to exit.
	 close(passive_fd);
	 handle_exit();
   }

   client_fd = accept ( passive_fd, NULL, NULL );    // Accept ( server_data, client_data, client_len )
                                                     // where client_data are optional metadata about
													 // special requests.
   
   if( client_fd >=0 ){                              // Mantain client count in order to exit cleanly
     printf("[+] Handling a new connection\n");
	 child_count++;                                  // and spawn the client handler.
	 handle_client( client_fd );   
   }

 }

}

void handle_client(int client_fd){
	int pid=fork();                                  // Fork and create child for client connection.

	if( pid<0  ) error("ERROR trying to fork.");
	if( pid>0  ) { close(client_fd); return; }       // lets free the client_fd in order to handle other conn.
    if( pid==0 ) {                                   // If PID is 0 we are in the child code.

	  char data_buff[1024];

	  memset (data_buff,'\0',sizeof(data_buff));     // Init buffer
	  
      strcpy(data_buff,"Hi, sir.\n");
	  write (client_fd,data_buff,sizeof(data_buff));
      
	  // shutdown(client_fd,SHUT_RDWR);               // This isn't needed but can be used to close only one way
	                                                  // for example using SHUT_RD
	  
	  close(client_fd);                               // This frees the socket descriptor
	  exit(0);
	}
}

void error(const char* msg){
 perror(msg);
 exit(1);
}

void handle_signal(int signal){
    printf("\n[!] Handled signal SIGINT whait for process to exit ***\n");
	printf("[!] If socket is blocking, this will not exit until a new-last connection.\n"); 
	// set the exit flag
	exit_flag=1;
}

void handle_exit(){
	 printf("\n[-] Exiting cleanly from %d childs\n   ",child_count);
	 
	 for( ; child_count>0 ; child_count-- ){ 
	   wait(); 
	   printf(".");
	 }
	 printf ("\n[!] Bye.\n");
	 exit(0);
}
