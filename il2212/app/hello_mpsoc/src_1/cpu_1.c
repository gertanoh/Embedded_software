#include <stdio.h>
#include "system.h"
#include "io.h"
#include <altera_avalon_mutex.h>
#include <altera_avalon_mutex_regs.h>
#define TRUE 2

extern void delay (int millisec);


int main()
{
  /*int * sharedmemory = (int *)0x102000;
  int a =&sharedmemory;
  
  printf("Hello from cpu_1!\n");
  alt_mutex_dev* mutex_0 = altera_avalon_mutex_open ("dev/mutex_0"); //bcbc
  altera_avalon_mutex_lock(mutex_0,1); //lock mutex_0 owner id = 0 
  for (a = 0; a < 5;a++)
  
  
  {printf("a = %d\n",a);
  
  }
  altera_avalon_mutex_unlock(mutex_0);
  
  
  printf("sharedmemory = %6x\n",sharedmemory);
  //printf("sharedmemory = %d\n",&a);
  *sharedmemory = sharedmemory + 0x1; //update address
  printf("sharedmemory uppdate = %6x\n",*sharedmemory);*/
   int b = 1;
  while (b>0) { /* ... */ 
  int * sharedmemory = (int *)0x102000;
  //int a = 5;
  
  alt_mutex_dev* mutex_0 = altera_avalon_mutex_open ("/dev/mutex_0"); //

  altera_avalon_mutex_lock(mutex_0,0); //lock mutex_0 owner id = 0 
  //*sharedmemory = 1;
  *sharedmemory = *sharedmemory + 1; //update value
 printf("sharedmemory = %6x\n",sharedmemory);
  printf("value = %d\n",*sharedmemory);
   
  altera_avalon_mutex_unlock(mutex_0);
   b--;
   }
  return 0;
}
