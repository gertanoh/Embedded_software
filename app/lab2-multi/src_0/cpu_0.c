
#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "system.h"
#include "images_alt.h"
#include "../shared_memory_mapping.h"
#include <altera_avalon_mutex.h>

#define DEBUG 1
#define SECTION_1 1



/* set sof
 * flags for synchronisation 
 */
 
 extern void delay (int millisec);

 /* get pointers */
volatile int* const flag_rgb = (volatile int*)FLAG_RGB_PIC;
volatile int* const flag_ascii = (volatile int*)FLAG_ASCI_PIC;
//volatile int* const square_addr = (volatile int*)
/* to ensure that cpu_0 has read the old ascii picture before we add a new one
 * we use masks, flag_ascii = 1111 when every cpu has put a picture 
 * we could have used another pointer, but this might be faster by saving another read/write for 
 * every quarter of the picture 
 */


void sram2sm_p3(unsigned char* base, unsigned char* output)
{
    
    int x;
    unsigned int  total_size = (base[0] * base[1] * 3) + 3;
    //3075;//32*32*3 + 3;
    unsigned int size_4b = total_size >> 2;
	
	// get the size bits
    unsigned int size_left  = total_size % 3;
    
    


    unsigned int *input = (unsigned int *)base;
    unsigned int *out = (unsigned int *)output;

    for(x=0; x<size_4b; x++){
        *out++ = *input++;
    }
	
	// write the shift part of the size
    base = (unsigned char *) input;
    output = (unsigned char *) out;
    for(x=0; x<size_left; x++){
	    *output++ = *base++;
    }

}

void write_to_sram(unsigned char* input, unsigned char* output){

    int x , y;

    unsigned char size_x = *input++;
	unsigned char size_y = *input++;
	input++;
	
	*output++ = size_x;
	*output++ = size_y;
	*output++ = 255;
	
	for(y = 0; y < size_y  ; y++){
	    for(x = 0; x < size_x  ; x++){
               *output++ = *input++;
	    }
	}

}


  
int main(void) {


    alt_mutex_dev* mutex_0;
	alt_mutex_dev* mutex_1;
	
	
	/* mutexes for synchronisation */
	
	
	mutex_0 = altera_avalon_mutex_open( "/dev/mutex_0" );
	mutex_1 = altera_avalon_mutex_open( "/dev/mutex_1" );
    
    /* reset pointers */
    while(altera_avalon_mutex_trylock(mutex_0, 1) != 0)	
	{	
	}
	*flag_rgb = 0;
	altera_avalon_mutex_unlock(mutex_0);
	
	while(altera_avalon_mutex_trylock(mutex_1, 1) != 0)	
	{	
	}
	*flag_ascii = 0;
                        
	altera_avalon_mutex_unlock(mutex_1);
    
    // output
    unsigned char output[18*18+3] __attribute__ ((aligned (4)));
    
    int k = 0;
    char current_image=0;
   #if DEBUG == 1
	/* Sequence of images for testing if the system functions properly */
    char number_of_images=10;
    unsigned char* img_array[10] = {circle20x20, circle24x24, circle32x22,circle32x32, circle40x28, 
    circle40x40, rectangle20x20, rectangle24x24, rectangle32x22,  rectangle40x40};
	#else
	/* Sequence of images for measuring performance */
    char number_of_images=3;
    unsigned char* img_array[3] = {circle32x32, rectangle32x32, circle32x32};
	#endif
  
    delay(1500);
    /* write first buffer picture */
    
    sram2sm_p3(img_array[current_image], (unsigned char*)RGB_ADDR);
    /* set flags to 4 */
    while(altera_avalon_mutex_trylock(mutex_0, 1) != 0)	
	{	
	}
	*flag_rgb = 4;				
	altera_avalon_mutex_unlock(mutex_0);

    

        
    while(1){
    
 
    PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);

       for(k = 0; k< 580;k++){
		   
		   
            /* wait for everyone to put picture on chip */
             while( *flag_rgb != 0);

            /* write another picure */
            current_image=(current_image+1) % number_of_images;
            sram2sm_p3(img_array[current_image], (unsigned char*)RGB_ADDR);
            /* set flags to 4 */
            while(altera_avalon_mutex_trylock(mutex_0, 1) != 0)	
	        {	
	        }
	        *flag_rgb = 4;				
	        altera_avalon_mutex_unlock(mutex_0);
			
			
            /*wait to read picture back to sram */
            while(*flag_ascii != 15);
            
            // write back picture
            write_to_sram((unsigned char*)ASCII_ADDR, output);
            altera_avalon_mutex_lock(mutex_1, 1);
            *flag_ascii = 0;
            altera_avalon_mutex_unlock(mutex_1);
            #if DEBUG == 1
            int i = 0, x ,y;
            int size_x = output[0];
            int size_y = output[1];
            
            printf("-------------------Picture of size : %d, %d\n", size_x, size_y );
             printf("\n");
          printf("\n");
            for(y = 0; y < size_y  ; y++){
                for(x = 0; x < size_x; x++){
                    printf("%c",output[i+3]);
                    i++;
                }
                printf("\n");
            }
            delay(1000);
            #endif
        }
        PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
        perf_print_formatted_report
	        (PERFORMANCE_COUNTER_0_BASE,            
	        ALT_CPU_FREQ,        // defined in "system.h"
	        1,                   // How many sections to print
	        "Performance Mode"        // Display-name of section(s).
	        ); 
	       
        delay(100);
		        
    }    
    return 0;
}
