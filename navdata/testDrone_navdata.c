// g++ -o testDrone_navdata testDrone_navdata.c 
// gcc -o testDrone_navdata testDrone_navdata.c 

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

int main(int argc, char *argv[]) {
  
  // check command line args //
  if(argc<2) {
    printf("usage : %s <AR.Drone's IP>\n", argv[0]);
    exit(1);
  }
  
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

  // initialize variables
  seq   = 1;
  flags = 0;
  socketsize = sizeof(droneAddr_navdata);
  
  //socket initialization
  // get server IP address (no check if input is IP address or DNS name)
  h = gethostbyname(argv[1]);
  if(h==NULL) {
    printf("\tunknown host '%s' \n", argv[1]);
    exit(1);
  }

  // create socket for each communication port
  createSocket(&sd1, &droneAddr_navdata, &clientAddr_navdata, h, DRONE_NAVDATA_PORT);
  createSocket(&sd2, &droneAddr_command, &clientAddr_command, h, DRONE_COMMAND_PORT);
  
  
  // start communication
  // tap drone's port: drone starts to send navdata in bootstrap mode
  printf("\tnavigation data start\n");
  sprintf(command, "\x01\x00");
  sendCommand(sd1, command, flags, droneAddr_navdata);
  
  // stop bootstrap mode
  printf("\tstop bootstrap mode\n"); 
  sprintf(command, "AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r",seq++);
  sendCommand(sd2, command, flags, droneAddr_command);
    
  // send ack to start navdata
  printf("\tsend ack\n");
  sprintf(command, "AT*CTRL=%d,0\r",seq++);
  sendCommand(sd2, command, flags, droneAddr_command);
  
  // send command to trim sensors
  printf("\ttrim sensors\n"); 
  sprintf(command, "AT*FTRIM=%d,\r",seq++);
  sendCommand(sd2, command, flags, droneAddr_command);
  
  // receive navdata.
  for (i=0;i<350;i++) {
    
    
    // sample command sequences for take-off, turn, and landing
    if (0 <= i && i < 100) {  // take off
        printf("send AT*REF:take off\n");
        sprintf(command, "AT*REF=%d,290718208\r",seq++);
        sendCommand(sd2, command, flags, droneAddr_command);
    } else if (100 <= i && i < 300){ // rotate
        printf("send AT*PCMD\n");
        arg1 = 0; arg2 = 0; arg3 = 0; arg4 = 0.1;
        sprintf(command, "AT*PCMD=%d,1,%d,%d,%d,%d\r",seq++,
                *(int32_t*)(&arg1), *(int32_t*)(&arg2),
                *(int32_t*)(&arg3), *(int32_t*)(&arg4));
        sendCommand(sd2, command, flags, droneAddr_command);
    } else { // land
        printf("send AT*REF:land\n");
        sprintf(command, "AT*REF=%d,290717696\r",seq++);
        sendCommand(sd2, command, flags, droneAddr_command);
    }

    
    // send watchdog if no command is sent to command port, so as to prevent drone from entering hover mode
    printf("send AT*COMWDG\n");
    sprintf(command, "AT*COMWDG=%d\r",seq++);
    sendCommand(sd2, command, flags, droneAddr_command);
        
    // tickle drone's port: drone send one packet of navdata in navdata_demo mode 
    sendCommand(sd1, "\x01\x00", flags, droneAddr_navdata);
    
    //receive data 
    memset( navdata, '\0', sizeof(navdata)); 
    navdata_size = recvfrom(sd1, navdata, sizeof(navdata), 0, (struct sockaddr *)&droneAddr_navdata, &socketsize);
    printf("received navdata %d bytes\n",navdata_size);
    
    // printf("decode navdata_struct %d bytes\n",sizeof(navdata_struct));
    memcpy(&navdata_struct, navdata, sizeof(navdata_struct));
    printf("navdata header:\n");
    printf("\t%13x:%s\n", navdata_struct.navdata_header.header, "Header"          );
    printf("\t%13x:%s\n", navdata_struct.navdata_header.state,  "drone's state"   );
    printf("\t%13d:%s\n", navdata_struct.navdata_header.seq,    "sequence number" );
    printf("\t%13d:%s\n", navdata_struct.navdata_header.vision, "vision flag"     );
    printf("\t%13d:%s\n", navdata_struct.navdata_option.id,     "Option1 ID"      );
    printf("\t%13d:%s\n", navdata_struct.navdata_option.size,   "Option1 size"    );
    printf("drone's state:\n");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  0))!=0, "FLY MASK");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  1))!=0, "VIDEO MASK");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  2))!=0, "VISION MASK");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  3))!=0, "CONTROL ALGO");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  4))!=0, "ALTITUDE CONTROL ALGO");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  5))!=0, "USER feedback");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  6))!=0, "Control command ACK");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  7))!=0, "Trim command ACK");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  8))!=0, "Trim running");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 <<  9))!=0, "Trim result");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 10))!=0, "Navdata demo");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 11))!=0, "Navdata bootstrap");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 12))!=0, "Motors status");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 13))!=0, "Communication Lost");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 14))!=0, "problem with gyrometers");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 15))!=0, "VBat low");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 16))!=0, "VBat high");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 17))!=0, "Timer elapsed");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 18))!=0, "Power");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 19))!=0, "Angles");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 20))!=0, "Wind");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 21))!=0, "Ultrasonic sensor");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 22))!=0, "Cutout system detection");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 23))!=0, "PIC Version number OK");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 24))!=0, "ATCodec thread");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 25))!=0, "Navdata thread");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 26))!=0, "Video thread");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 27))!=0, "Acquisition thread");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 28))!=0, "CTRL watchdog");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 29))!=0, "ADC Watchdog");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 30))!=0, "Communication Watchdog");
    printf("\t%13d:%s\n",(navdata_struct.navdata_header.state & (1 << 31))!=0, "Emergency landing");
    printf("navdata_demo:\n");
    printf("\t%13d:%s\n",   navdata_struct.navdata_option.ctrl_state,             "Control State");
    printf("\t%13d:%s\n",   navdata_struct.navdata_option.vbat_flying_percentage, "battery");
    printf("\t%13.3f:%s\n", navdata_struct.navdata_option.theta,                  "pitch angle");
    printf("\t%13.3f:%s\n", navdata_struct.navdata_option.phi,                    "roll  angle");
    printf("\t%13.3f:%s\n", navdata_struct.navdata_option.psi,                    "yaw   angle");
    printf("\t%13d:%s\n",   navdata_struct.navdata_option.altitude,               "altitude");
    printf("\t%13.3f:%s\n", navdata_struct.navdata_option.vx,                     "estimated vx");
    printf("\t%13.3f:%s\n", navdata_struct.navdata_option.vy,                     "estimated vy");
    printf("\t%13.3f:%s\n", navdata_struct.navdata_option.vz,                     "estimated vz");
    
    // reset emergency if it's in emergency mode
    if ((navdata_struct.navdata_header.state & (1 << 31))!=0) {
      sprintf(command, "AT*REF=%d,290717952\r",seq++);
      sendCommand(sd2, command, flags, droneAddr_command);
    }
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

