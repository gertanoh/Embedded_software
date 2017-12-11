#include <stdio.h>
#include "system.h"
#include "io.h"
#include "altera_avalon_fifo_util.h"
#include "altera_avalon_fifo_regs.h"
#include "altera_avalon_mutex.h"




#define TRUE 1

extern void delay (int millisec);

//delay(100000);

/* Cpu 2 writes odd values from 1 to 99 to fifo 0 */

int main()
{
    int i = 0;
    int value = 1;
    printf("Hello from cpu_2!\n");
    alt_mutex_dev* mutex_1 = altera_avalon_mutex_open(MUTEX_1_NAME);
    
  
    while (TRUE) {
        altera_avalon_mutex_lock(mutex_1, 1);
        /* It would be nice to generate, random number but don't know if we have the library */
        for(i = 0 ; i < 5 ; i++){
            altera_avalon_fifo_write_fifo(FIFO_1_IN_BASE, FIFO_1_IN_CSR_BASE, value);
            printf("Wrote : %d\n", value);
            value += 2;
            if(value >= 100){
            /* reset the value because can not display a 3 digit number on one hex */
            value = 1;
        }
        }
        altera_avalon_mutex_unlock(mutex_1);
        
        delay(30);
        
    }
    return 0;
}
