#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "io.h"
#include "altera_avalon_performance_counter.h"
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_fifo_util.h"
#include "altera_avalon_fifo_regs.h"
#include "altera_avalon_mutex.h"



#define TRUE 1

extern void delay (int millisec);
int writeToShared(int *data, int cpu_id);
int readFromShared();

/* write data to core_4, read data from core_4 and write data to core_0 */

int main()
{
    int tab[4] = {10, 15 , 52, 23};
    int i = 0;
    printf("Hello from cpu_3!\n");
    altera_mutex_dev *mutex_2 = altera_avalon_mutex_open(MUTEX_2_NAME);
    
    
    while (TRUE) {
        /* write to core 4 */
        writeToShared(tab, 4);
        delay(5);
        /* read from core_4 */
        readFromShared();
        
        /* write to core_0 */
        writeToShared(tab, 0);
        
        dealy(30);
        for(i = 0 ; i < 4; i++){
            if(i %2 == 0){
                tab[i] -= 1;
            }
            else {
                tab[i] += 2;
            }
        }
    }
    return 0;
}


int readFromShared(int cpu_id){

    /* data exchanged between core_3 and core_4 at SHARED_BASE + 2048
    */
    
    unsigned *char value;
    altera_avalon_mutex_lock(mutex_2, 1);
    value = (unsigned char*) SHARED_ONCHIP_BASE + 1 + 2048;
    for(i = 0 ; i < 4; i++){
        printf("Receivied from the core_ %d : %d\n", cpu_id, *(value + i));
    }
    altera_avalon_mutex_unlock(mutex_2);
    
    return res;
}


