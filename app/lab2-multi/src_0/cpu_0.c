// Skeleton for lab 2
// 
// Task 1 writes periodically RGB-images to the shared memory
//
// No guarantees provided - if bugs are detected, report them in the Issue tracker of the github repository!

#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "system.h"
#include "images.h"
#include "altera_avalon_mutex.h"
#define DEBUG 1

#define SECTION_1 1

extern void delay (int millisec);
unsigned char sqrtImproved(int x);
//void image_to_ascii();

/*
 * Example function for copying a p3 image from sram to the shared on-chip mempry
 */
void sram2sm_p3(unsigned char* base)
{
	int x, y;
	unsigned char* shared;

	shared = (unsigned char*) SHARED_ONCHIP_BASE;

	int size_x = *base++;
	int size_y = *base++;
	int max_col = *base++;

	*shared++  = size_x;
	*shared++  = size_y;
	*shared++  = max_col;
	for(y = 0; y < size_y; y++)
	for(x = 0; x < size_x; x++)
	{
		*shared++ = *base++; 	// R
        *shared++ = *base++;	// G
		*shared++ = *base++;	// B
   
	}
}
void graycolor ()
{
    int x ,y;
    unsigned char*  gray = (unsigned char*) SHARED_ONCHIP_BASE;
    int size_x= *gray++;
    int size_y = *gray++;
    int max_col = *gray++;
    unsigned char* alpha = (unsigned char*) SHARED_ONCHIP_BASE;
    double L ;
    alpha= gray;
    double red = 0.3125;
    double green = 0.5625;
    double blue = 0.125;
    for(y = 0; y < size_y; y++)
	    for(x = 0; x < size_x; x++)
	    {
	        
	        L = red * *gray++ + green * *gray++ + blue * *gray++;
	        *alpha++ = L;
        }
}
void resize()
{
   int x ,y; //Henry
    unsigned char*  gray = (unsigned char*) SHARED_ONCHIP_BASE;
    int size_x= *gray++;
    int size_y = *gray++;
    int max_col = *gray++;
    unsigned char* resize_imaged ;
    resize_imaged = gray;
    
    for(y = 0; y < size_y /2 ; y++)
        for( x = 0; x < size_x / 2; x++){
            *resize_imaged++ = ( *gray + *(gray + 1) + *(gray + size_x * sizeof(unsigned char)) //
            + *(gray + size_x * (sizeof(unsigned char)) + 1) ) / 4 ;
            //printf("%d\n", *resize_imaged);
            gray = gray + 2;
        }
}
void edge_detection()
{
    int x ,y;
    unsigned char*  pixel_pointer = (unsigned char*) SHARED_ONCHIP_BASE;

    
    int size_x= (*pixel_pointer++);
    size_x = size_x >> 1;
    int size_y = (*pixel_pointer++);
    size_y = size_y >>1;
    *pixel_pointer++;
    
    // we put the data at the end of the old so no overwriting
    unsigned char* edge_value = (unsigned char*) ( pixel_pointer + ((size_x)) * ((size_y)) );    
    /* kernels */
    int X[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1,0, 1}
        };
            
    int Y[3][3] = {
            {-1, -2, -1},
            {0, 0, 0},
            {1, 2, 1}
        };
        
    int tmp_x;
    int tmp_y;
    int tmp;
    for( y = 0; y < size_y; y++)
        for( x = 0; x < size_x; x++){
            // apply the sobel operator on each pixel
            tmp_x = (X[0][0] * (*pixel_pointer)) + (X[0][1] * (*(pixel_pointer + 1))) + (X[0][2] * (*(pixel_pointer + 2)))+
        (X[0][0] * (*(pixel_pointer + size_x))) + (X[0][1] * (*(pixel_pointer + 1+ size_x))) + (X[0][2] * (*(pixel_pointer + 2 + size_x)))+
(X[0][0] * (*(pixel_pointer + 2 * size_x))) + (X[0][1] * (*(pixel_pointer + 1+ 2 * size_x))) + (X[0][2] * (*(pixel_pointer + 2 + 2 * size_x)));
            
            tmp_y = (Y[0][0] * (*pixel_pointer)) + (Y[0][1] * (*(pixel_pointer + 1))) + (Y[0][2] * (*(pixel_pointer + 2)))+
        (Y[0][0] * (*(pixel_pointer + size_x))) + (Y[0][1] * (*(pixel_pointer + 1+ size_x))) + (Y[0][2] * (*(pixel_pointer + 2 + size_x)))+
(Y[0][0] * (*(pixel_pointer + 2 * size_x))) + (Y[0][1] * (*(pixel_pointer + 1+ 2 * size_x))) + (Y[0][2] * (*(pixel_pointer + 2 + 2 * size_x)));

            tmp = tmp_x * tmp_x + tmp_y * tmp_y ;
            *edge_value = sqrtImproved(tmp);
            edge_value++;
            *pixel_pointer++;
        }
     
     
}     
unsigned char sqrtImproved(int x){

    /*use of unsigned char because max color is 255 */
    /* square root function */
    double rt = 1, ort = 0;
    while(ort != rt){
        ort = rt;
        rt = ((x/rt) + rt ) / 2;
    }
    
    return (unsigned char) rt;
}

void image_to_ascii(){
    
    int x ,y;  
    unsigned char*  pixel_pointer = (unsigned char*) SHARED_ONCHIP_BASE;

    // The picture is now half sized
    int size_x= (*pixel_pointer++);
    int size_y = (*pixel_pointer++);
    *pixel_pointer++;
    unsigned char* toshared;
    toshared= pixel_pointer;
    // the new data are after the old data
    //pixel_pointer += size_x * size_y ; 
    //char symbols[] = {' ','-','.','_', ':', '=', '+','*', 'i','x','#','$','g','&','%','@' };
    char symbols[] =  {'@','%','&','g','$','#','x','i','*','+', '=', ':', '_', '.', '-',' ' };
    
    int saturation;        
    for( y = 0; y < size_y; y++){
        for( x = 0; x < size_x; x++){
            char symbol = '\0';
            saturation = (int) ( ( (*pixel_pointer++)/255.0 )* 15 );
            symbol = symbols[saturation];
            *toshared++ = symbol;  
        }
        
    }
}
void writeback()
{
    int x ,y;  
    unsigned char*  pixel_pointer = (unsigned char*) SHARED_ONCHIP_BASE;

    // The picture is now half sized
    int size_x= (*pixel_pointer++);
    int size_y = (*pixel_pointer++);
    *pixel_pointer++;

    unsigned char writeback [size_x][size_y];
    for(y = 0; y <size_y; y++){
        for(x = 0; x < size_x; x++){
            writeback[x][y] = *pixel_pointer++;
           // printf("%c",writeback[x][y]);
        }
       // printf("\n");
    }

}

int main(void) {

  	

	char current_image=0;
	
	#if DEBUG == 1
	/* Sequence of images for testing if the system functions properly */
	char number_of_images=10;
	unsigned char* img_array[10] = {img1_24_24, img1_32_22, img1_32_32, img1_40_28, img1_40_40, 
			img2_24_24, img2_32_22, img2_32_32, img2_40_28, img2_40_40 };
	#else
	/* Sequence of images for measuring performance */
	char number_of_images=3;
	unsigned char* img_array[3] = {img1_32_32, img2_32_32, img3_32_32};
	#endif
    alt_mutex_dev *mutex_0 = 	 altera_avalon_mutex_open(MUTEX_0_NAME);		
	while (1)
	{ 
		/* Extract the x and y dimensions of the picture */
		unsigned char i = *img_array[current_image];
		unsigned char j = *(img_array[current_image]+1);

		#if DEBUG == 1
            altera_mutex_lock(mutex_0, 1);
    		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		    /* Measurement here */

		    sram2sm_p3(img_array[current_image]);
		    graycolor();
	        resize();
	        edge_detection();	
            image_to_ascii();
            writeback();
            altera_unlock(mutex_0);
            
		    PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
		    /* Print report */
		    perf_print_formatted_report
		    (PERFORMANCE_COUNTER_0_BASE,            
		    ALT_CPU_FREQ,        // defined in "system.h"
		    1,                   // How many sections to print
		    "Debug mode"        // Display-name of section(s).
		    );   

		   	
		    /* delay */
		    delay(100);
	    
	    #else
	        int k;
	        
    		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
		    PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
		    PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		    
	        for(k = 0; k< 100; k++){
	            /* Measurement here */
		        sram2sm_p3(img_array[current_image]);
		        graycolor();
	            resize();
	            edge_detection();	
                image_to_ascii();
                writeback();
            }
            PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
		    /* Print report */
		    perf_print_formatted_report
		    (PERFORMANCE_COUNTER_0_BASE,            
		    ALT_CPU_FREQ,        // defined in "system.h"
		    1,                   // How many sections to print
		    "Performance mode"        // Display-name of section(s).
		    ); 
            
            delay(50000);
        #endif
         /* Increment the image pointer */
	    current_image=(current_image+1) % number_of_images;		   
    } 
     
  return 0;
}
