#include <stdio.h>
#include <arpa/inet.h>

#include <string.h>

#include "navdata.h"

// seperate processes for cmd/nav socket communications

#define NAVDATA_PORT 5554
#define COMMAND_PORT 5556

#define MAX_NAVDATA 1024

#define IP_ADDR "192.168.1.1"
#define SIZEOF_ARRAY(a) (sizeof(a) / sizeof(a[0]))

#define SEND_COMMAND_SLEEP 250000

#define _0_05   1028443341
#define _0_1    1036831949
#define _0_2    1045220575
#define _0_5    1056964608
#define _1_0    1065353216

#define _n0_05 -1119040307
#define _n0_1  -1110651699
#define _n0_2  -1102263091
#define _n0_5  -1090519040
#define _n1_0  -1082130432

#include "drone_comm.h"

Drone_Comm::Drone_Comm() 
{
        // FIXME ?Arbitrary?
        seq_num = 1;
}

/*
  Opens the command communication socket with the drone.  Assigns the
  socket file descriptor to cmd_sock_fd.  Returns receiver_addr.
*/
void Drone_Comm::open_comm()
{
        // Navdata socket creation and initialization
        nav_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        navdata_addr.sin_family = AF_INET;
        
        if( 0 == inet_aton( IP_ADDR,  &navdata_addr.sin_addr ) ) 
        {
                printf("Crap!, Init failed\n");
                close_comm();
                return;
        }
        navdata_addr.sin_port = htons( NAVDATA_PORT );

        // Command socket creation and initialization
        cmd_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        command_addr.sin_family = AF_INET;
        
        if( 0 == inet_aton( IP_ADDR,  &command_addr.sin_addr ) ) 
        {
                printf("Crap!, Init failed\n");
                close_comm();
                return;
        }
        command_addr.sin_port = htons( COMMAND_PORT );

        // send command to trim sensors
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*FTRIM=%d,\r",seq_num++);
        send_motion_cmd();
}

/* 
  Close the command communication socket with the drone
 */
void Drone_Comm::close_comm()
{
        close(nav_sock_fd);
        close(cmd_sock_fd);
}

/*
  Send an AT*PCMD flight motion command over the communication socket
  to the drone
 */
void Drone_Comm::send_motion_cmd()
{
        sendto(cmd_sock_fd, cmd_buf, SIZEOF_ARRAY(cmd_buf), 0, (struct sockaddr*)&command_addr, sizeof(command_addr));
        usleep(SEND_COMMAND_SLEEP);
}

void Drone_Comm::send_nav_cmd()
{
        sendto(nav_sock_fd, cmd_buf, SIZEOF_ARRAY(cmd_buf), 0, (struct sockaddr*)&navdata_addr, sizeof(navdata_addr));
        usleep(SEND_COMMAND_SLEEP);
}

// START AT*REF Commands
/*
  Send the command for the drone to take off
 */
void Drone_Comm::takeoff()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*REF=%d,290718208\r", seq_num++);
        send_motion_cmd();
}

/*
  Send the command for the drone to land
 */
void Drone_Comm::land()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*REF=%d,290717696\r", seq_num++);
        send_motion_cmd();
}

/*
  Send the command for the drone to hover
 */
void Drone_Comm::hover()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,0,0\r", seq_num++);
        send_motion_cmd();
}
// END AT*REF Commands

// START AT*PCMD flight motion commands

// FIXME determine how many degrees this would have the drone rotate

void Drone_Comm::alt_gain()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,%d,0\r", seq_num++, _0_5);
        send_motion_cmd();
}

void Drone_Comm::alt_lose()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,%d,0\r", seq_num++, _n0_5);
        send_motion_cmd();
}

void Drone_Comm::rotate_cw()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,0,%d\r", seq_num++, _0_5);
        send_motion_cmd();
}

void Drone_Comm::rotate_ccw()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,0,%d\r", seq_num++, _n0_5);
        send_motion_cmd();
}

void Drone_Comm::pitch_forward()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,%d,0,0\r", seq_num++, _n0_1);
        send_motion_cmd();
}

void Drone_Comm::pitch_backward()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,%d,0,0\r", seq_num++, _0_1);
        send_motion_cmd();
}

void Drone_Comm::roll_right()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,%d,0,0,0\r", seq_num++, _0_1);
        send_motion_cmd();
}

void Drone_Comm::roll_left()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,%d,0,0,0\r", seq_num++, _n0_1);
        send_motion_cmd();
}

void Drone_Comm::start_navdata()
{
    socklen_t socketsize;

    socketsize = sizeof(navdata_addr);

    // start communication
    // tap drone's port: drone starts to send navdata in bootstrap mode
    snprintf(cmd_buf, MAX_CMD_LEN, "\x01\x00");
    send_nav_cmd();

    // stop bootstrap mode    
    snprintf(cmd_buf, MAX_CMD_LEN, "AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", seq_num++);    
    send_motion_cmd();

    // send ack to start navdata
    snprintf(cmd_buf, MAX_CMD_LEN, "AT*CTRL=%d,0\r",seq_num++);
    send_motion_cmd();
    
    // send command to trim sensors
    snprintf(cmd_buf, MAX_CMD_LEN, "AT*FTRIM=%d,\r",seq_num++);
    send_motion_cmd();
    
    // send watchdog if no cmd_buf is sent to cmd_buf port, so as to prevent drone from entering hover mode
    sprintf(cmd_buf, "AT*COMWDG=%d\r",seq_num++);
    send_motion_cmd();

    // tickle drone's port: drone send one packet of navdata in navdata_demo mode 
    snprintf(cmd_buf, MAX_CMD_LEN, "\x01\x00");
    send_nav_cmd();
}

int Drone_Comm::query_battery()
{
    int navdata[MAX_NAVDATA];  
    int navdata_size;
    socklen_t socketsize;
    navdata_t navdata_struct;
    
    // receive data 
    memset( navdata, '\0', sizeof(navdata)); 
    navdata_size = recvfrom(nav_sock_fd, navdata, sizeof(navdata), 0, (struct sockaddr *)&navdata_addr, &socketsize);
    // printf("received navdata %d bytes\n",navdata_size);
    
    // printf("decode navdata_struct %d bytes\n",sizeof(navdata_struct));
    memcpy(&navdata_struct, navdata, sizeof(navdata_struct));
    // printf("%2d%s\n", navdata_struct.navdata_option.vbat_flying_percentage, "%");

    return navdata_struct.navdata_option.vbat_flying_percentage;
}
// END AT*PCMD flight motion Commands

