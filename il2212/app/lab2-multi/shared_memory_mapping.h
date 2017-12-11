

/*

 * Memory mapping for shared memory
 * We define here the addresses of the pointer 
 */
 
 
 
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H


#include "system.h"


// write picture at base address of the shared memory */
#define RGB_ADDR 0x102000


// address where each cpu_1..4 put the back the picture resized, base address + 40*40*3
#define RESIZED_ADDR 0x1032D0
#define ASCII_ADDR 0x103470


/* flags addresses */


// Mutex 0
#define FLAG_RGB_PIC 0x103610

// Mutex 1
#define FLAG_ASCI_PIC 0x103614



/* flags for synchronisation before sobel */
#define FLAG_FINISH_INTER_1 0x103618
#define FLAG_FINISH_INTER_2 0x10361C
#define FLAG_FINISH_INTER_3 0x103700
#define FLAG_FINISH_INTER_4 0x103704

#endif

