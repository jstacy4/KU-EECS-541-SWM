#include <stdio.h>
#include "drone_comm.h"

int main () 
{
        Drone_Comm c;

        c.open_comm();

        c.takeoff();
        c.hover();
        sleep(25);
        c.land();

        c.close_comm();        
}
