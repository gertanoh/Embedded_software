

/*Code for
* SIngle BAre 
*/



#include <stdio.h>
#include <stdlib.h>
#include "altera_avalon_performance_counter.h"
#include "system.h"
//#include "images.h"
#include "images_alt.h"
#include "io.h"

#define DEBUG 1

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
 * function for interpolating gray scale image 
 */
void interpolation()
{

	int x ,y; 
    unsigned char*  gray = (unsigned char*) SHARED_ONCHIP_BASE;
    int size_x= *gray++;
    int size_y = *gray++;
    gray++;
    unsigned char* resize_imaged ;
    resize_imaged = gray;
    

    
    for(y = 0; y < size_y ; y+=2)
        for( x = 0; x < size_x; x+=2){
            *resize_imaged++ = ( *(gray+x) + *(gray + x + 1) + *(gray + (y+1)*size_x+x) 
            + *(gray + (y+1)*size_x+ x + 1) ) >> 2 ;
        }
	
	//update size 
	gray = (unsigned char*) SHARED_ONCHIP_BASE;
	*gray++ = size_x >> 1;
	*gray++ = size_y >> 1;
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
            
   	        output_matrix[x-1][y-1] = ascii_art(gx[x-1][y-1]*gx[x-1][y-1] + gy[x-1][y-1]*gy[x-1][y-1]);
	    }
	}
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	*shared++ = size_x - 2;
	*shared++ = size_y - 2;
	*shared++ = 255;
	for(y = 0; y < size_y-2; y++){
	    for(x = 0; x < size_x-2; x++){
	        *shared++ = output_matrix[x][y];
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
        symbol = ';';
    }
    else if (value <= 26896){
        symbol = '*';
    }
    else if (value <= 42025){
        symbol = 's';
    }
    else if (value <= 60516){
        symbol = 't';
    }
    else if (value <= 82369){
        symbol = 'I';
    }
    else if (value <= 107584){
        symbol = '(';
    }
    else if (value <= 136161){
        symbol = 'j';
    }
    else if (value <= 168100){
        symbol = 'u';
    }
    else if (value <= 203401){
        symbol = 'V';
    }
    else if (value <= 242064){
        symbol = 'w';
    }
    else if (value <= 284089){
        symbol = 'b';
    }
    else if (value <= 329476){
        symbol = 'R';
    }
    else if (value <= 378225){
        symbol = 'g';
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
			printf("%c",*base);	
			#endif
			base++;
		}
		#if DEBUG == 1
	    printf("\n");
		#endif
	}
}

int main(void) {

    char current_image=0; delay(1);
#if DEBUG == 1
/* Sequence of images for testing if the system functions properly */
    unsigned char out[403];
    char number_of_images=10;
    /*unsigned char* img_array[10] = {img1_24_24, img1_32_22, img1_32_32, img1_40_28, img1_40_40, 
    img2_24_24, img2_32_22, img2_32_32, img2_40_28, img2_40_40 };*/
    unsigned char* img_array[10] = {circle20x20, circle24x24, circle32x22,circle32x32, circle40x28, 
    circle40x40, rectangle20x20, rectangle24x24, rectangle32x22,  rectangle40x40};
#else
/* Sequence of images for measuring performance */
    char number_of_images=3;
    unsigned char out[260];
    unsigned char* img_array[3] = {circle32x32, rectangle32x32, circle32x32};
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
    interpolation();
    edge_detection();
    write_sram(out);            
            
   PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
   /* Print report */
   perf_print_formatted_report
   (PERFORMANCE_COUNTER_0_BASE,            
   ALT_CPU_FREQ,        // defined in "system.h"
   1,                   // How many sections to print
   "Debug mode"        // Display-name of section(s).
   );   

   /* delay */
   delay(1000);
   
   #else
       int k;
       
    PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
   
       /* Measurement here */
       for(k = 0; k < 100; k++){
            sram2sm_p3(img_array[current_image]);
            grayscale();
            interpolation();
            edge_detection();
            write_sram(out);
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
