#include <stdio.h>
#include "drone_comm.h"
#include <sys/types.h>
#include <signal.h>

int main () 
{
    Drone_Comm c;
    FILE *fp;
    int level = 0;

    /*
    c.open_comm();
    c.start_navdata();

    for( int i = 0; i < 50; i++ )
    {
        level = c.query_battery();
        
        fflush(stdout);
        printf("%d\n", level);
    }

    c.close_comm();
    */

    fp = fopen("text.txt", "w");

    c.open_comm();
    c.start_navdata();
    // c.takeoff();
    // c.hover();

    sleep(2);

    // c.land();

    pid_t pid = fork();

    // Child Process
    if( pid == 0 )
    {
        fflush(stdout);
        printf("%s\n", "Child");

        int i = 0;

        while( true )
        {
            // fflush(stdout);            
            // printf("%d", i);
            // i++;
            level = c.query_battery();

            sleep(1);

            if( level != 0 )
            {
                fflush(stdout);
                printf("%d. %d\n", i, level);
                i++;
                // sleep(1);
            }
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
        c.close_comm();    
    }    

    c.land();        
    c.close_comm();    
}


    // fprintf(fp, "%d", level);
    
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


