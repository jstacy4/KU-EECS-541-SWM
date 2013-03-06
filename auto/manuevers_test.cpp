#include <stdio.h>
#include "drone_comm.h"

int main () 
{
        Drone_Comm c;

        c.open_comm();

        c.takeoff();
        c.hover();

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.alt_gain();
        }

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.pitch_forward();
                
        }

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.roll_right();
        }

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.rotate_cw();
        }

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.pitch_backward();
                
        }

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.roll_left();
        }

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.rotate_ccw();
        } 

        sleep(1.5);

        for( int i = 0; i < 15; i++ )
        {
                c.alt_gain();
        }

        sleep(1.5);
        c.land();

        c.close_comm();        
}

// alt_gain();
// alt_lose();

// pitch_forward();
// pitch_backward();

// rotate_cw();
// rotate_ccw();

// roll_right();
// roll_left();
