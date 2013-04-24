#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus 
extern "C" {
#endif

#include "navdata.h"
#include "com.h"

#ifdef __cplusplus 
}
#endif

#define DRONE_COMMAND_PORT 5556
#define DRONE_NAVDATA_PORT 5554

#define MAX_COMMAND        1024
#define MAX_NAVDATA        1024

int main(int argc, char *argv[]) 
{  
    // declare variables
    int                 i, seq;
    int                 navdata_size;
    int                 sd1, sd2, flags;
    int                 navdata[MAX_NAVDATA];  
    char                command[MAX_COMMAND];
    float               arg1, arg2, arg3, arg4;
    struct hostent      *h;
    struct sockaddr_in  droneAddr_navdata;
    struct sockaddr_in  droneAddr_command;
    struct sockaddr_in  clientAddr_navdata;
    struct sockaddr_in  clientAddr_command;
    
    navdata_t          navdata_struct;
    socklen_t          socketsize;

    FILE * outfile = NULL;

    // initialize variables
    seq   = 1;
    flags = 0;
    socketsize = sizeof(droneAddr_navdata);
    
    if( 2 == argc )
    {
        outfile = fopen(argv[1], "w");
        if( 0 == outfile )
        {
            printf("Could not open specified output file");
            exit(1);
        }
    }
  
    //socket initialization
    // get server IP address (no check if input is IP address or DNS name)
    h = gethostbyname("192.168.1.1");
    if( NULL == h )
    {
        printf("\tunknown host '%s' \n", argv[1]);
        exit(1);
    }
    
    // create socket for each communication port
    createSocket(&sd1, &droneAddr_navdata, &clientAddr_navdata, h, DRONE_NAVDATA_PORT);
    createSocket(&sd2, &droneAddr_command, &clientAddr_command, h, DRONE_COMMAND_PORT);
        
    // start communication
    // tap drone's port: drone starts to send navdata in bootstrap mode
    // printf("\tnavigation data start\n");
    sprintf(command, "\x01\x00");
    sendCommand(sd1, command, flags, droneAddr_navdata);
    
    // stop bootstrap mode
    // printf("\tstop bootstrap mode\n"); 
    sprintf(command, "AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r",seq++);
    sendCommand(sd2, command, flags, droneAddr_command);
    
    // send ack to start navdata
    // printf("\tsend ack\n");
    sprintf(command, "AT*CTRL=%d,0\r",seq++);
    sendCommand(sd2, command, flags, droneAddr_command);
    
    // send command to trim sensors
    // printf("\ttrim sensors\n"); 
    sprintf(command, "AT*FTRIM=%d,\r",seq++);
    sendCommand(sd2, command, flags, droneAddr_command);
    
    // send watchdog if no command is sent to command port, so as to prevent drone from entering hover mode
    // printf("send AT*COMWDG\n");
    sprintf(command, "AT*COMWDG=%d\r",seq++);
    sendCommand(sd2, command, flags, droneAddr_command);
    
    // tickle drone's port: drone send one packet of navdata in navdata_demo mode 
    sendCommand(sd1, "\x01\x00", flags, droneAddr_navdata);
    
    //receive data 
    memset( navdata, '\0', sizeof(navdata)); 
    navdata_size = recvfrom(sd1, navdata, sizeof(navdata), 0, (struct sockaddr *)&droneAddr_navdata, &socketsize);
    // printf("received navdata %d bytes\n",navdata_size);
    
    // printf("decode navdata_struct %d bytes\n",sizeof(navdata_struct));
    memcpy(&navdata_struct, navdata, sizeof(navdata_struct));
    printf("%2d%s\n", navdata_struct.navdata_option.vbat_flying_percentage, "%");
    
    if( NULL != outfile)
    {
        fprintf(outfile, "%2d%s\n", navdata_struct.navdata_option.vbat_flying_percentage, "%");
    }
    
    // reset emergency if it's in emergency mode
    if ((navdata_struct.navdata_header.state & (1 << 31))!=0) 
    {
        sprintf(command, "AT*REF=%d,290717952\r",seq++);
        sendCommand(sd2, command, flags, droneAddr_command);
    }
    
    // close socket
    if (close(sd1)<0) {
        printf("\tcannnot close socket for navdata port");
        exit(1);
    }
    if (close(sd2)<0) {
        printf("\tcannnot close socket for command port");
        exit(1);
    }
    
    return 0;    
}

