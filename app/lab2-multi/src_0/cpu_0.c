
#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "system.h"
#include "images_alt.h"


#define DEBUG 1
#define READ_OFFSET 4000
#define SECTION_1 1


#define PICTURE_OFFSET 16
/* set sof
 * flags for synchronisation 
 */
 
 
volatile unsigned char* flag_finish_sram = (volatile unsigned char*)SHARED_ONCHIP_BASE;


volatile unsigned char* flag_finish_sobel_1 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+1);
volatile unsigned char* flag_finish_sobel_2 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+2);
volatile unsigned char* flag_finish_sobel_3 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+3);
volatile unsigned char* flag_finish_sobel_4 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+4);

volatile unsigned char* flag_finish_read_1 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+9);
volatile unsigned char* flag_finish_read_2 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+10);
volatile unsigned char* flag_finish_read_3 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+11);
volatile unsigned char* flag_finish_read_4 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+12);


unsigned char output[13][13];


void sram2sm_p3(int* base)
{
	int  y;
	//converting image to gray scale
	// write 4 bytes to the shared memory
	int *shared = (int*) (SHARED_ONCHIP_BASE+PICTURE_OFFSET);
	for(y = 0; y < 768; y++)
	{
		*shared++ = *base++; 	// write of 4 bytes

	}

}

void write_to_sram(){

    int x , y;
    unsigned char* base = (unsigned char*)(SHARED_ONCHIP_BASE+READ_OFFSET);
    unsigned char size_x_off = 13;
	unsigned char size_y_off = 13 ;
	for(y = 0; y < size_y_off  ; y++){
	    for(x = 0; x < size_x_off  ; x++){
	        output[x][y] = *base++;
	        #if DEBUG == 1
			printf("%c",output[x][y]);	
			#endif
	    }
	    #if DEBUG == 1
	    printf("\n");
		#endif
	}

}


    /*unsigned int volatile * const finish_pointer = (unsigned int *)FLAG_SOBEL_RAM_ADDR;
    unsigned int volatile * const write_pointer = (unsigned int *)FLAG_WRITE_RAM_ADDR;*/
int main(void) {


    int k = 0;
    char current_image=0;
    /* Sequence of images for measuring performance */
    char number_of_images=3;
    unsigned char* img_array[3] = {circle32x32, rectangle32x32, circle32x32};    
  
    delay(2000);
    /* write first buffer picture */
    sram2sm_p3(img_array[current_image]);

    PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
     *flag_finish_sram = 1; 
        
    while(1){
    
   
        for(k = 0; k <500;k++){
            /* wait for everyone to put picture on chip */
             while(!( *flag_finish_read_1 == 1 && *flag_finish_read_2 == 1
                && *flag_finish_read_3 == 1 && *flag_finish_read_4 == 1 ));
            *flag_finish_sram = 0;
            /* write another picure */
             current_image=(current_image+1) % number_of_images;
            sram2sm_p3(img_array[current_image]);
            *flag_finish_sram = 1;
            /*wait to read picture back to sram */
            while(!( *flag_finish_sobel_1 == 1 && *flag_finish_sobel_2 == 1
                && *flag_finish_sobel_3 == 1 && *flag_finish_sobel_4 == 1 ));
            write_to_sram();
            delay(1000);
            current_image=(current_image+1) % number_of_images;
        }
            PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
            perf_print_formatted_report
		        (PERFORMANCE_COUNTER_0_BASE,            
		        ALT_CPU_FREQ,        // defined in "system.h"
		        3,                   // How many sections to print
		        "Performance Mode","Update pointer write","pointer finish"        // Display-name of section(s).
		        ); 
		        
		        delay(10000);
		        
    }    
    return 0;
}
