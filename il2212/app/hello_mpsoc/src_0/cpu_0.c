#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "io.h"
#include <altera_avalon_mutex.h>
#include <altera_avalon_mutex_regs.h>
#include "stdlib.h"
#include "alt_types.h"

#define TRUE 1

//extern void delay (int millisec);

int main(void)
{


 //printf("Sharedmemory!\n"); 
  printf("Hello from cpu_0, test!\n");
   //printf("Sharedmemory!\n"); 
  
 /* int * sharedmemory = (int *)0x102000;
  //int a = 0;
  int a = &sharedmemory;
  alt_mutex_dev* mutex_1 = altera_avalon_mutex_open ("dev/mutex_1"); //
  altera_avalon_mutex_lock(mutex_1,1); //lock mutex_0 owner id = 0 
  
  a = 1;
 
  altera_avalon_mutex_unlock(mutex_1);
  
  //printf("sharedmemory = %d\n",a);
  printf("sharedmemory = %6x\n",sharedmemory);
  printf("sharedmemory = %d\n",a);
  sharedmemory ++; //update address
  printf("sharedmemory uppdate = %6x\n",sharedmemory);*/
  
  //int *pointer;
  /*
  while (TRUE)
  {
  int init_button;
  int init_read; 
  alt_u8 button = 0x2;
  init_button = IOADDR_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
   //init_read =IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
   //init_read=init_button;
   //if (init_read == 0)
   
   init_read=IOWR_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE,1);
   pointer = init_button;
   IOWR_ALTERA_AVALON_PIO_DATA(LEDS_GREEN_BASE, init_button);
   printf("varde = %d\n",*pointer);
   
  }
 //int b = 1;
  //while (b>0) {
  /* if (init_read == 1)
   {
  int * sharedmemory = (int *)0x102000;
  //int a = 5;
  
  alt_mutex_dev* mutex_0 = altera_avalon_mutex_open ("/dev/mutex_0"); //
  altera_avalon_mutex_lock(mutex_0,0); //lock mutex_0 owner id = 0 
  
  int i;
  for ( i = 0; i< 5;i++)
  {
  *sharedmemory = *sharedmemory + 1; //update value
   
  
 printf("sharedmemory = %6x\n",sharedmemory);
  printf("value = %d\n",*sharedmemory);
   
  altera_avalon_mutex_unlock(mutex_0);
  }
  }
  
   else 
   {
  printf("pres the Button\n");
  }  
  b--;
  }*/
  /* Declare volatile pointers to I/O registers (volatile means that IO load and store
instructions (e.g., ldwio, stwio) will be used to access these pointer locations) */
volatile int * red_LED_ptr = (int *) 0x105170; // red LED address
volatile int * green_LED_ptr = (int *) 0x105190; // green LED address
volatile int * HEX3_HEX0_ptr = (int *) 0x105160; // HEX3_HEX0 address
volatile int * HEX7_HEX4_ptr = (int *) 0x105150; // HEX7_HEX4 address
volatile int * SW_switch_ptr = (int *) 0x105180; // SW slider switch address
volatile int * KEY_ptr = (int *) 0x105140; // pushbutton KEY address
int HEX_bits = 0x0000000F; // pattern for HEX displays
int SW_value, KEY_value;
while(1)
{
SW_value = *(SW_switch_ptr); // read the SW slider switch values
*(red_LED_ptr) = SW_value; // light up the red LEDs
KEY_value = *(KEY_ptr); // read the pushbutton KEY values
*(green_LED_ptr) = KEY_value; // light up the green LEDs
if (KEY_value != 0) // check if any KEY was pressed
{
//HEX_bits = SW_value; // set pattern using SW values
while (*KEY_ptr); // wait for pushbutton KEY release
}
*(HEX3_HEX0_ptr) = HEX_bits; // display pattern on HEX3 ... HEX0
*(HEX7_HEX4_ptr) = HEX_bits; // display pattern on HEX7 ... HEX4
if (HEX_bits & 0x80000000) /* rotate the pattern shown on the HEX displays */
HEX_bits = (HEX_bits << 1) | 1;
else
HEX_bits = HEX_bits << 1;
//for (delay_count = 500000; delay_count != 0; −−delay_count); // delay loop
extern void delay (int millisec);
}
 // return 0;
}
