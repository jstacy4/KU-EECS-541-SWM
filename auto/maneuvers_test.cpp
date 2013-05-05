#include <stdio.h>
#include "drone_comm.h"
#include <sys/types.h>
#include <signal.h>
#include <time.h>

int main () 
{
    Drone_Comm c;
    FILE *fp;

    fp = fopen("text.txt", "w");

    c.start_gps();
    c.open_comm();
    c.start_navdata();

    sleep(2);

    pid_t pid = fork();

    // Child Process
    if( pid == 0 )
    {
        fflush(stdout);
        printf("%s\n", "Child");

        while( true )
        {
            sleep(1);

            printf("\n");
            c.print_battery_level();
            c.print_gps_data();
        }        
    }

    // Error
    else if( pid <= 0 )
    {
        printf("%s", "Failed to fork");
    }

    // Parent Process
    else
    {
        fflush(stdout);
        printf("%s\n", "Parent");

        c.takeoff();
        c.hover();
        
        sleep(4);

        for( int i = 0; i < 15; i++ )
        {
            c.rotate_ccw();        
        }

        kill(pid, SIGTERM);

        sleep(0.5);

        c.land();        
        c.stop_gps();
        c.close_comm();    
    }
}


    // fprintf(fp, "%d", battery_level);
    
    // for( int i = 0; i < 5; i++ )
    // {
    //     c.pitch_forward();
    //     // c.rotate_ccw();
    //     // c.alt_gain();
    //     // c.pitch_forward();
    //     // c.roll_right();
    //     // c.rotate_cw();
    //     // c.pitch_backward();
    //     // c.roll_left();
    //     // c.rotate_ccw();
    // }


