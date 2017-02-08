#include <stdio.h>
#include "system.h"
#include "io.h"
#include "altera_avalon_fifo.h"
#include "altera_avalon_fifo_regs.h"
#include "altera_avalon_fifo_util.h"
#include <altera_avalon_mutex.h>

#define TRUE 1

extern void delay (int millisec);


/*
alt_u32 address = 0x1051e4;
int altera_avalon_fifo_init(alt_u32 0x1051e4, alt_u32 ienable,
                            alt_u32 emptymark, alt_u32 fullmark)
{
    if(altera_avalon_fifo_clear_event(0x1051e4, ALTERA_AVALON_FIFO_EVENT_ALL) != ALTERA_AVALON_FIFO_OK)
    {
        return ALTERA_AVALON_FIFO_EVENT_CLEAR_ERROR;
    }

    if( altera_avalon_fifo_write_ienable(0x1051e4, ienable) != ALTERA_AVALON_FIFO_OK)
    {
         return ALTERA_AVALON_FIFO_IENABLE_WRITE_ERROR;
    }

    if( altera_avalon_fifo_write_almostfull(0x1051e4, fullmark) != ALTERA_AVALON_FIFO_OK)
    {
        return ALTERA_AVALON_FIFO_THRESHOLD_WRITE_ERROR;
    }
  
    if( altera_avalon_fifo_write_almostempty(0x1051e4, emptymark) != ALTERA_AVALON_FIFO_OK)
    {
        return ALTERA_AVALON_FIFO_THRESHOLD_WRITE_ERROR;
    }

    return ALTERA_AVALON_FIFO_OK;
}
*/

int main()
{
  printf("Hello from cpu_2!\n");
  
    /* init the fifo */
   
   int init_value = 0;
   alt_mutex_dev* mutex_1 = altera_avalon_mutex_open ("/dev/mutex_1"); //
  altera_avalon_mutex_lock(mutex_1,0);
   init_value = altera_avalon_fifo_init(FIFO_0_IN_BASE, 0, 1, FIFO_0_IN_FIFO_DEPTH - 4);
   if(init_value != 0){
        alt_u32 data = 17;
        int res = altera_avalon_fifo_write_fifo(FIFO_0_IN_BASE, FIFO_0_IN_CSR_BASE, data);
        if(res != 0) {
               printf(" fifo writing did not work \n");
       }
       else {
            printf("write ok ");
         }
    }
    else {
        printf(" init no ");
        }
  altera_avalon_mutex_unlock(mutex_1);
  while (TRUE) { /* ... */ }
  return 0;
  
}
