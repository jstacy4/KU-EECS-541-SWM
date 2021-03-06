#ifndef DRONE_COMM_H
#define DRONE_COMM_H

#include <netinet/in.h>
#include <unistd.h>
#include <gps.h>
#include <time.h>

//FIXME change this?
#define MAX_CMD_LEN 150

class Drone_Comm
{
public:
        Drone_Comm();
        
        void open_comm();
        void close_comm();

        void send_motion_cmd();
        void send_nav_cmd();

        void start_gps();
        void stop_gps();

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

        void start_navdata();
        int query_battery();
        void print_battery_level();

        void update_gps_data();
        void print_gps_data();

        struct gps_data_t gpsdata;
        
private:
        int cmd_sock_fd;
        int nav_sock_fd;
        int seq_num;
        char cmd_buf[MAX_CMD_LEN];
        struct sockaddr_in command_addr;
        struct sockaddr_in navdata_addr;

        time_t rawtime;
        struct tm * timeinfo;
        int hour;
};

#endif
