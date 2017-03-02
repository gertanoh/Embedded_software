

/*Code for
* SIngle BAre 
*/



#include <stdio.h>
#include <stdlib.h>
#include "altera_avalon_performance_counter.h"
#include "system.h"
#include "images.h"
#include "io.h"

#define DEBUG 0

#define SECTION_1 1


extern void delay (int millisec);

unsigned char ascii_art(int value);

void sram2sm_p3(int* base)
{
	int x, y;
	unsigned char size_x = *base;
	unsigned char size_y = ( (*base) >> 8 );
	int size ;
	size =  ( (size_x * size_y * 3 ) >> 2) + 1;
	//converting image to gray scale
	// write 4 bytes to the shared memory
	/* THe idea is for performace mode
	* we only work with 32 * 32 pictures and the max color is the same 255
	* so need to store them 
	* we use the avalon bus size to write 4 bytes in one shot
	*/
	int *shared = (int*) (SHARED_ONCHIP_BASE);
	for(y = 0; y < size; y++)
	{
		*shared++ = *base++; 	// write of 4 bytes

	}

}
void grayscale(){
    int x, y;
	unsigned char* shared;

	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	int size_x = *shared++;
	int size_y = *shared++;
    shared++;
    unsigned char* gray ;
    gray = shared;
    int temp = 0;
	for(y = 0; y < size_y; y++)
	    {
	    for(x = 0; x < size_x; x++)
	    {
	        temp = (*gray)<<2; // R
	        temp += *gray++; // R
	        temp += (*gray)<<3; // G
	        temp += *gray++; // G
	        temp = temp >> 4;
	        temp += (*gray++)>> 3; //B
		    *shared++ = temp;
	    }
	}
}
/*
void grayscale(){
    int x, y;
	unsigned char* shared;

	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	int size_x = *shared++;
	int size_y = *shared++;
    shared++;
    unsigned char output_matrix [40][40];
    int temp = 0;
	for(y = 0; y < size_y; y++)
	    {
	    for(x = 0; x < size_x; x++)
	    {
	        temp = (*shared)<<2; // R
	        temp += *shared++; // R
	        temp += (*shared)<<3; // G
	        temp += *shared++; // G
	        temp = temp >> 4;
	        temp += (*shared++)>> 3; //B
		    output_matrix[x][y] = temp;
	    }
	}
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	shared += 3;
	for(y = 0; y < size_y; y++){
	    for(x = 0; x < size_x; x++){
	        *shared++ = output_matrix[x][y];
        }
    }
}
*/

/*
 * function for interpolating gray scale image 
 */
void interpolation()
{
	int x, y;
	unsigned char* shared;
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	unsigned char output_matrix [20][20];
	unsigned char input_matrix [40][40];
	int size_x = *shared++;
	int size_y = *shared++;
	shared++;
    int i, j;
	for(y = 0, j = 0; y < (size_y-1); y+=2,j++){
	    for(x = 0, i = 0; x < (size_x-1); x+=2, i++){
	    	   output_matrix[i][j] = (input_matrix[x][y] +input_matrix[x+1][y]
	    	   +input_matrix[x][y+1] + input_matrix[x+1][y+1] ) >> 2;
	    }
	}
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	*shared++ = size_x >> 1;
	*shared++ = size_y >> 1;
	*shared++ = 225;
	for(y = 0; y <  size_y >> 1; y++){
	    for(x = 0; x <  size_x >> 1; x++){
	        *shared++ = output_matrix[x][y];
        }
    }
}

/*
 * function for edge detection 
 */
void edge_detection()
{
	int x, y;
	int gx[18][18];
	int gy[18][18];
	unsigned char output_matrix[18][18];
	unsigned char input_matrix[20][20];
	unsigned char* shared;
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	int size_x = *shared++;
	int size_y = *shared++;
	shared++;
    
    for(y = 0; y < size_y; y++){
        for(x = 0; x < size_x; x++){
            input_matrix[x][y] = *shared++;
        }
    }
	
	for(y = 1; y < size_y-1; y++){
	    for(x = 1; x < size_x-1; x++){
	        gx[x-1][y-1] = (-input_matrix[x-1][y-1] + input_matrix[x-1][y+1]) 
	            + ( -(input_matrix[x][y-1]<<1) +(input_matrix[x][y+1]<<1) ) + 
	        ( -input_matrix[x+1][y-1] +input_matrix[x+1][y+1] );
		
	        gy[x-1][y-1] = (input_matrix[x-1][y-1] + (input_matrix[x-1][y]<<1) + input_matrix[x-1][y+1] )+
	                        (-(input_matrix[x+1][y-1]) -  (input_matrix[x+1][y]<<1) -input_matrix[x+1][y+1] );
	                        gx[x-1][y-1] = (-input_matrix[x-1][y-1] + v[x-1][y+1]) 
	            + ( -(input_matrix[x][y-1]<<1) +(input_matrix[x][y+1]<<1) ) + 
	        ( -input_matrix[x+1][y-1] +input_matrix[x+1][y+1] );
		
	       /* gy[x-1][y-1] = (input_matrix[x-1][y-1] + (input_matrix[x-1][y]<<1) + input_matrix[x-1][y+1] )+
	                        (-(input_matrix[x+1][y-1]) -  (input_matrix[x+1][y]<<1) -input_matrix[x+1][y+1] );*/
            
	       if(gx[x-1][y-1]< 0) gx[x-1][y-1]= -gx[x-1][y-1];
   	       if(gy[x-1][y-1]< 0) gy[x-1][y-1]= -gy[x-1][y-1];
   	        output_matrix[x-1][y-1] = ascii_art(gx[x-1][y-1] + gy[x-1][y-1]);
	    }
	}
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	*shared++ = size_x - 2;
	*shared++ = size_y - 2;
	*shared++ = 255;
	for(y = 0; y < size_y-2; y++){
	    for(x = 0; x < size_x-2; x++){
	        *shared++ = output_matrix[x-1][y-1];
        }
    }
}



/* new square root function 
 * we know that the maximun value reached is 437650
  * sqrt(437650) is 661
  *  661 / 16 = 41
  * we map value from 0-41*41 , 42*42 - 123*123
  */ 
unsigned char ascii_art(int value)
{
     unsigned char symbol = '\0';
    if(value <= 1681){
        symbol = ' ';
    }
    else if (value <= 6724){
        symbol = '.';
    }
    else if (value <= 15129){
        symbol = ':';
    }
    else if (value <= 26896){
        symbol = '=';
    }
    else if (value <= 42025){
        symbol = '+';
    }
    else if (value <= 60516){
        symbol = '*';
    }
    else if (value <= 82369){
        symbol = 'c';
    }
    else if (value <= 107584){
        symbol = 'i';
    }
    else if (value <= 136161){
        symbol = 'x';
    }
    else if (value <= 168100){
        symbol = 'e';
    }
    else if (value <= 203401){
        symbol = 'o';
    }
    else if (value <= 242064){
        symbol = '&';
    }
    else if (value <= 284089){
        symbol = '8';
    }
    else if (value <= 329476){
        symbol = '#';
    }
    else if (value <= 378225){
        symbol = '%';
    }
    else if (value > 378225) {
        symbol = '@';
    }
    return symbol;
   
}
void write_sram(unsigned char *base){
    int x, y;
	unsigned char* shared;
	shared = (unsigned char*) (SHARED_ONCHIP_BASE);
	int SIZE_X = *shared++;
	int SIZE_Y = *shared++;
	shared++;
	for(y = 0; y < (SIZE_Y); y++){
		for(x = 0; x < (SIZE_X ); x++)
		{
			*base = *shared++;
			#if DEBUG == 1
			//printf("%c",*base);	
			#endif
			base++;
		}
		#if DEBUG == 1
	    //printf("\n");
		#endif
	}
}

int main(void) {

char current_image=0;
#if DEBUG == 1
/* Sequence of images for testing if the system functions properly */
    unsigned char out[403];
    char number_of_images=10;
    unsigned char* img_array[10] = {img1_24_24, img1_32_22, img1_32_32, img1_40_28, img1_40_40, 
    img2_24_24, img2_32_22, img2_32_32, img2_40_28, img2_40_40 };
#else
/* Sequence of images for measuring performance */
    char number_of_images=3;
    unsigned char out[260];
    unsigned char* img_array[3] = {img1_32_32, img2_32_32, img3_32_32};
#endif

while (1)
{ 
/* Extract the x and y dimensions of the picture */
unsigned char i = *img_array[current_image];
unsigned char j = *(img_array[current_image]+1);

#if DEBUG == 1
    PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
   /* Measurement here */
    sram2sm_p3(img_array[current_image]);
    grayscale();
    //interpolation();
    //edge_detection();
   // write_sram(out);            
            
   PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
   /* Print report */
   perf_print_formatted_report
   (PERFORMANCE_COUNTER_0_BASE,            
   ALT_CPU_FREQ,        // defined in "system.h"
   1,                   // How many sections to print
   "Debug mode"        // Display-name of section(s).
   );   

   /* delay */
   delay(500);
   
   #else
       int k;
       
    PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
   
       /* Measurement here */
       for(k = 0; k < 100; k++){
            PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 2);
            sram2sm_p3(img_array[current_image]);
            PERF_END(PERFORMANCE_COUNTER_0_BASE, 2);
            PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 3);
            grayscale();
            PERF_END(PERFORMANCE_COUNTER_0_BASE, 3);
            PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 4);
            interpolation();
            PERF_END(PERFORMANCE_COUNTER_0_BASE, 4);
            PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 5);
            edge_detection();
            PERF_END(PERFORMANCE_COUNTER_0_BASE, 5);
            PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, 6);
            write_sram(out);
            PERF_END(PERFORMANCE_COUNTER_0_BASE, 6);
            PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
             perf_print_formatted_report
		        (PERFORMANCE_COUNTER_0_BASE,            
		        ALT_CPU_FREQ,        // defined in "system.h"
		        6,                   // How many sections to print
		        "Total","Sram","grayscle","inte","sobel","write"        // Display-name of section(s).
		        );
	        // Print report 
	        delay(10000);
            current_image=(current_image+1) % number_of_images; 
        }
        PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
        perf_print_formatted_report
		        (PERFORMANCE_COUNTER_0_BASE,            
		        ALT_CPU_FREQ,        // defined in "system.h"
		        1,                   // How many sections to print
		        "Performance Mode"        // Display-name of section(s).
		        ); 
        #endif
         /* Increment the image pointer */
   current_image=(current_image+1) % number_of_images;   
    } 
  return 0;
}
