#include <stdio.h>
#include "drone_comm.h"

int main () 
{
    Drone_Comm c;
    FILE *fp;
    int level = 0;

    fp = fopen("text.txt", "w");

    c.open_comm();
    
    // c.takeoff();
    // c.hover();
    
    // sleep(2);

    do
    {
        level = c.query_battery();
        fflush(stdout);
        printf("%d", level);
    } while (level == 0);

    fprintf(fp, "%d", level);
    
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

    // sleep(2);
    // c.land();
    
    c.close_comm();        
}
