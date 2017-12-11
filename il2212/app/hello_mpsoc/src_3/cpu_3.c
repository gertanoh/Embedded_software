#include <stdio.h>
#include "system.h"
#include "io.h"
#include "altera_avalon_fifo.h"
#include "altera_avalon_fifo_regs.h"
#include "altera_avalon_fifo_util.h"
#include <altera_avalon_mutex.h>

#define TRUE 1

extern void delay (int millisec);

int main()
{
  printf("Hello from cpu_3!\n");
  
  
  int init_value = 0;
  alt_mutex_dev* mutex_1 = altera_avalon_mutex_open ("/dev/mutex_1"); //
  altera_avalon_mutex_lock(mutex_1,0);
   init_value = altera_avalon_fifo_init(FIFO_0_OUT_BASE, 0, 1, FIFO_0_OUT_FIFO_DEPTH - 8);
   if(init_value != 0){
        int res = altera_avalon_fifo_read_fifo(FIFO_0_OUT_BASE - 8, FIFO_0_OUT_CSR_BASE- 8);
        
            printf("value read from fifo : %d\n", res);
        
    }
    else {
        printf(" init no ");
        }
        altera_avalon_mutex_unlock(mutex_1);
  while (TRUE) { /* ... */ }
  return 0;
}
