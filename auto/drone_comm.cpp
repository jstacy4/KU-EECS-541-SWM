#include <stdio.h>
#include <arpa/inet.h>

// seperate processes for cmd/nav socket communications

#define NAVDATA_PORT 5554
#define COMMAND_PORT 5556

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

// FIXME seq numbers need to be dynamic!
// char const TAKEOFF[] = "AT*REF=101,290718208\r";
// char const LAND[]    = "AT*REF=500,290717696\r";
// char const HOVER[]   = "AT*PCMD=201,1,0,0,0,0\r";

#include "drone_comm.h"

Drone_Comm::Drone_Comm() 
{
        // FIXME ?Arbitrary?
        seq_num = 100;
}

/*
  Opens the command communication socket with the drone.  Assigns the
  socket file descriptor to cmd_sock_fd.  Returns receiver_addr.
*/
void Drone_Comm::open_comm()
{
        // Navdata socket creation and initialization
        nav_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        receiver_addr.sin_family = AF_INET;
        
        if( 0 == inet_aton( IP_ADDR,  &receiver_addr.sin_addr ) ) 
        {
                printf("Crap!, Init failed\n");
                close_comm();
                return;
        }
        receiver_addr.sin_port = htons( COMMAND_PORT );

        // Command socket creation and initialization
        cmd_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        receiver_addr.sin_family = AF_INET;
        
        if( 0 == inet_aton( IP_ADDR,  &receiver_addr.sin_addr ) ) 
        {
                printf("Crap!, Init failed\n");
                close_comm();
                return;
        }
        receiver_addr.sin_port = htons( COMMAND_PORT );
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
void Drone_Comm::send_motion_comm()
{
        sendto(cmd_sock_fd, cmd_buf, SIZEOF_ARRAY(cmd_buf), 0, (struct sockaddr*)&receiver_addr, sizeof(receiver_addr));
        usleep(SEND_COMMAND_SLEEP);
}

// START AT*REF Commands
/*
  Send the command for the drone to take off
 */
void Drone_Comm::takeoff()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*REF=%d,290718208\r", seq_num++);
        send_motion_comm();

        // snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,%d,0,0,0\r", seq_num++, _n0_1);
        // send_motion_comm();
        // char const TAKEOFF[] = "AT*REF=101,290718208\r";

        // sendto(cmd_sock_fd, TAKEOFF, SIZEOF_ARRAY(TAKEOFF), 0, (struct sockaddr*)&receiver_addr,sizeof(receiver_addr));
}

/*
  Send the command for the drone to land
 */
void Drone_Comm::land()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*REF=%d,290717696\r", seq_num++);
        send_motion_comm();

        // char const LAND[]    = "AT*REF=500,290717696\r";

        // sendto(cmd_sock_fd, LAND, SIZEOF_ARRAY(LAND), 0,(struct sockaddr*)&receiver_addr,sizeof(receiver_addr));
}

/*
  Send the command for the drone to hover
 */
void Drone_Comm::hover()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,0,0\r", seq_num++);
        send_motion_comm();

        // char const HOVER[]   = "AT*PCMD=201,1,0,0,0,0\r";

        // sendto(cmd_sock_fd, HOVER, SIZEOF_ARRAY(HOVER), 0,(struct sockaddr*)&receiver_addr,sizeof(receiver_addr));
}
// END AT*REF Commands

// START AT*PCMD flight motion commands

// FIXME determine how many degrees this would have the drone rotate

void Drone_Comm::alt_gain()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,%d,0\r", seq_num++, _0_5);
        send_motion_comm();
}

void Drone_Comm::alt_lose()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,%d,0\r", seq_num++, _n0_5);
        send_motion_comm();
}

void Drone_Comm::rotate_cw()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,0,%d\r", seq_num++, _0_5);
        send_motion_comm();

        // int angle = 1056964608;
 
        // snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,0,%d\r", seq_num++, angle);
        // send_motion_comm();
}

void Drone_Comm::rotate_ccw()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,0,0,%d\r", seq_num++, _n0_5);
        send_motion_comm();
}

void Drone_Comm::pitch_forward()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,%d,0,0\r", seq_num++, _n0_1);
        send_motion_comm();

        // int angle = -1110651699;
 
        // snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,%d,0,0\r", seq_num++, angle);
        // send_motion_comm();
}

void Drone_Comm::pitch_backward()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,0,%d,0,0\r", seq_num++, _0_1);
        send_motion_comm();
}

void Drone_Comm::roll_right()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,%d,0,0,0\r", seq_num++, _0_1);
        send_motion_comm();
}

void Drone_Comm::roll_left()
{
        snprintf(cmd_buf, MAX_CMD_LEN, "AT*PCMD=%d,1,%d,0,0,0\r", seq_num++, _n0_1);
        send_motion_comm();
}
// END AT*PCMD flight motion Commands

