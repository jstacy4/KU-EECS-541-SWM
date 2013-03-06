#ifndef DRONE_COMM_H
#define DRONE_COMM_H

#include <netinet/in.h>
#include <unistd.h>

//FIXME change this?
#define MAX_CMD_LEN 150

class Drone_Comm
{
public:
        Drone_Comm();
        
        void open_comm();
        void close_comm();

        void send_motion_comm();

        void takeoff();
        void land();
        void hover();

        void alt_gain();
        void alt_lose();

        void pitch_forward();
        void pitch_backward();

        void rotate_cw();
        void rotate_ccw();

        void roll_right();
        void roll_left();
        
private:
        int cmd_sock_fd;
        int nav_sock_fd;
        int seq_num;
        char cmd_buf[MAX_CMD_LEN];
        struct sockaddr_in receiver_addr;
};

#endif
