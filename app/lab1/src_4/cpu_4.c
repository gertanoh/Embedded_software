/* 
 * "Small Hello World" example. 
 * 
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example 
 * designs. It requires a STDOUT  device in your system's hardware. 
 *
 * The purpose of this example is to demonstrate the smallest possible Hello 
 * World application, using the Nios II HAL library.  The memory footprint
 * of this hosted application is ~332 bytes by default using the standard 
 * reference design.  For a more fully featured Hello World application
 * example, see the example titled "Hello World".
 *
 * The memory footprint of this example has been reduced by making the
 * following changes to the normal "Hello World" example.
 * Check in the Nios II Software Developers Manual for a more complete 
 * description.
 * 
 * In the SW Application project (small_hello_world):
 *
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 * In System Library project (small_hello_world_syslib):
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 *    - Define the preprocessor option ALT_NO_INSTRUCTION_EMULATION 
 *      This removes software exception handling, which means that you cannot 
 *      run code compiled for Nios II cpu with a hardware multiplier on a core 
 *      without a the multiply unit. Check the Nios II Software Developers 
 *      Manual for more details.
 *
 *  - In the System Library page:
 *    - Set Periodic system timer and Timestamp timer to none
 *      This prevents the automatic inclusion of the timer driver.
 *
 *    - Set Max file descriptors to 4
 *      This reduces the size of the file handle pool.
 *
 *    - Check Main function does not exit
 *    - Uncheck Clean exit (flush buffers)
 *      This removes the unneeded call to exit when main returns, since it
 *      won't.
 *
 *    - Check Don't use C++
 *      This builds without the C++ support code.
 *
 *    - Check Small C library
 *      This uses a reduced functionality C library, which lacks  
 *      support for buffering, file IO, floating point and getch(), etc. 
 *      Check the Nios II Software Developers Manual for a complete list.
 *
 *    - Check Reduced device drivers
 *      This uses reduced functionality drivers if they're available. For the
 *      standard design this means you get polled UART and JTAG UART drivers,
 *      no support for the LCD driver and you lose the ability to program 
 *      CFI compliant flash devices.
 *
 *    - Check Access device drivers directly
 *      This bypasses the device file system to access device drivers directly.
 *      This eliminates the space required for the device file system services.
 *      It also provides a HAL version of libc services that access the drivers
 *      directly, further reducing space. Only a limited number of libc
 *      functions are available in this configuration.
 *
 *    - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */

#include "sys/alt_stdio.h"
#include <stdio.h>
#include "system.h"
#include "io.h"
#include "altera_avalon_fifo_util.h"
#include "altera_avalon_fifo_regs.h"
#include "altera_avalon_mutex.h"

/*
int main()
{ 
  alt_putstr("Hello cpu_4!\n");

  // Event loop never exits. 
  while (1);

  return 0;
}
*/


int writeToShared(int *data, int cpu_id);
int readFromShared();

/* write data to core_4, read data from core_4 and write data to core_0 */

alt_mutex_dev *mutex_2;
int main()
{
    int tab[4] = {9, 17 , 25, 48};
    int i = 0;
    alt_putstr("Hello cpu_4!\n"); 
    mutex_2 = altera_avalon_mutex_open(MUTEX_2_NAME);
    
    
    while (1) {
    
        delay(10);
        /* read from core_4 */
        readFromShared();
        
        /* write to core 4 */
        writeToShared(tab, 4);
        
        /* write to core_0 */
        writeToShared(tab, 0);
        
        delay(30);
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

int writeToShared(int *data, int cpu_id){

    unsigned char* writeAddress;
    int offset = 0;
    int i = 0;
    
    altera_avalon_mutex_lock(mutex_2, 1);
    if(cpu_id == 0){
        /* core 0*/
        offset = 1024 + 4;
    }
    else {
        /* core 4 */
        offset = 2048;
    }
    writeAddress = (unsigned char*) SHARED_ONCHIP_BASE + 1 + offset;
    for(i = 0 ; i < 4; i++){
        *(writeAddress + i) = *(data + i);
    }
    
    /* set cpu_id */
    //IOWR_8DIRECT(SHARED_ONCHIP_BASE, cpu_id);
    
    altera_avalon_mutex_unlock(mutex_2);
    return 0;
}
int readFromShared(){

    /* data exchanged between core_3 and core_4 at SHARED_BASE + 2048
    */
    
    unsigned char* value;
    int i = 0;
    altera_avalon_mutex_lock(mutex_2, 1);
    value = (unsigned char*) SHARED_ONCHIP_BASE + 1 + 2048;
    for(i = 0 ; i < 4; i++){
        printf("Receivied from the core_3 : %d\n", *(value + i));
    }
    altera_avalon_mutex_unlock(mutex_2);
    
    return 0;
}


