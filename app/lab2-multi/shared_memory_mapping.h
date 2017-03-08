

/*

 * Memory mapping for shared memory
 * We define here the addresses of the pointer 
 */
 
 
 
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H


#include "system.h"


// write picture at base address of the shared memory */
#define RGB_ADDR 0x102000


// address where each cpu_1..4 put the back the picture resized, base address + 32*32*3
#define RESIZED_ADDR 0x102C10
#define ASCII_ADDR 0x102D20


/* flags addresses */


// Mutex 0
#define FLAG_RGB_PIC 0x102DF4

// Mutex 1
#define FLAG_ASCI_PIC 0x102DF8



/* flags for synchronisation before sobel */
#define FLAG_FINISH_INTER_1 0x102DFC
#define FLAG_FINISH_INTER_2 0x102E10
#define FLAG_FINISH_INTER_3 0x102E14
#define FLAG_FINISH_INTER_4 0x102E18

#endif

