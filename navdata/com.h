#include <unistd.h>
#include <time.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// create socket for designated address and port
void createSocket(int *sd, struct sockaddr_in *droneAddr, 
                  struct sockaddr_in *clientAddr, struct hostent *h, int port);

// send command to designated address and port
void sendCommand(int socket, char* command, int flag, struct sockaddr_in droneAddr);



// create socket for designated address and port
void createSocket(int *sd, struct sockaddr_in *droneAddr, 
                  struct sockaddr_in *clientAddr, struct hostent *h, int port) {
  
  // create structure for ardrone address & port
  droneAddr->sin_family = h->h_addrtype;
  droneAddr->sin_port   = htons(port);
  memcpy((char *) &(droneAddr->sin_addr.s_addr), h->h_addr_list[0], h->h_length);
  
  // create structure for this client
  clientAddr->sin_family = AF_INET;
  clientAddr->sin_addr.s_addr = htonl(INADDR_ANY);
  clientAddr->sin_port = htons(0);
  
  // socket creation for NAV_PORT
  *sd = socket(AF_INET,SOCK_DGRAM,0);
  if(sd<0) {
    printf("%s: cannot open socket \n", h->h_addr_list[0]);
    exit(1);
  }
  
  // bind client's the port and address
  if(bind(*sd, (struct sockaddr *) clientAddr, sizeof(*clientAddr))<0) {
    printf("%d: cannot bind port\n", port);
    exit(1);
  }
  
}

// send command to designated address and port
void sendCommand(int socket, char* command, int flag, struct sockaddr_in droneAddr) {
  if(sendto(socket, command, strlen(command)+1, flag,  (struct sockaddr *) &droneAddr, sizeof(droneAddr))<0) {
    printf("can not send data\n");
    close(socket);
    exit(1);
  }
  
  // wait a while to adjust command interval
  struct timespec wait_command;
  wait_command.tv_sec  = 0;
  wait_command.tv_nsec = 40000;  
  nanosleep(&wait_command, NULL);
  
}

