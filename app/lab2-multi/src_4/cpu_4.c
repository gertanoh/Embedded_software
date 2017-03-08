

 
#include <stdio.h>
#include "system.h"
#include "altera_avalon_mutex.h"
#include "../shared_memory_mapping.h"



extern void delay (int millisec);




/* set sof
 * flags for synchronisation 
 */
 
 
volatile int* const flag_rgb =(volatile int*) FLAG_RGB_PIC;

volatile int* const flag_ascii =(volatile int*) FLAG_ASCI_PIC;

volatile int* const flag_inter_1 = FLAG_FINISH_INTER_1;
volatile int* const flag_inter_2 = FLAG_FINISH_INTER_2;
volatile int* const flag_inter_3 = FLAG_FINISH_INTER_3;
volatile int* const flag_inter_4 = FLAG_FINISH_INTER_4;


unsigned char input_matrix[18*18];
unsigned int x_size;
unsigned int y_size;
unsigned int x_offset;
unsigned int skip_offset;



void shared2_chip_gray(unsigned char* input, unsigned char* output )
{
	int x, y;
	unsigned int temp = 0, value = 0;
    
    
    // here fixed size 
    x_size = 16;
    y_size = 16;
    x_offset = 1584;//32*16*3 + 16*3; // it is cpu_4, fourth quarter, 
    skip_offset = x_size ;

	
	// move pointer
	input += x_offset + 3;
	for(y = 0; y < y_size ; y++){
	    for(x = 0; x < x_size  ; x++){
            temp = *input++;
            value = ( ( temp<< 2) + temp ) >> 4; // R
            temp = *input++;
            value += ( ( temp << 3) + temp)>> 4; //G
            
            temp = *input++; // B
            
            // write to output
            *output++ = (unsigned char) ( value + (temp >> 3));
            
	    }
	    
	    // we increment by offset 
	    input += ( (x_size<<1) + x_size );
	}

}


void interpolation(unsigned char* in, unsigned char* out)
{
	int x, y;
	int x1,x2, y1, y2;
	x_size = x_size >> 1;
	y_size = y_size >> 1;
	
	// calculate offset
	skip_offset = x_size;
	x_offset = 136;//16*8+8;
	
	
	out+= x_offset;
	
    for(y = 0; y < y_size; y++){
        for(x = 0; x < x_size; x++){
          
          x1 = *(in+16);
          x2 = (*in++);
          y1 = *(in+16);
          y2 = (*in++);
          *out++ = (unsigned char)((x1 + x2 + y1 + y2)>>2);

        }

    in += 16;
    out += skip_offset;

  }
	
	    
}


/* new square root function 
 * we know that the maximun value reached is 437650
  * sqrt(437650) is 661
  *  661 / 16 = 41
  * we map value from 0-41 , 42 - 123
  */ 
  

unsigned char ascii_art(int value)
{
     unsigned char symbol = '\0';
    if(value <= 41){
        symbol = ' ';
    }
    else if (value <= 82){
        symbol = '.';
    }
    else if (value <= 123){
        symbol = ';';
    }
    else if (value <= 164){
        symbol = '*';
    }
    else if (value <= 205){
        symbol = 's';
    }
    else if (value <= 246){
        symbol = 't';
    }
    else if (value <= 287){
        symbol = 'I';
    }
    else if (value <= 328){
        symbol = '(';
    }
    else if (value <= 369){
        symbol = 'j';
    }
    else if (value <= 410){
        symbol = 'u';
    }
    else if (value <= 451){
        symbol = 'V';
    }
    else if (value <= 492){
        symbol = 'w';
    }
    else if (value <= 533){
        symbol = 'b';
    }
    else if (value <= 574){
        symbol = 'R';
    }
    else if (value <= 615){
        symbol = 'g';
    }
    else if (value > 615) {
        symbol = '@';
    }
    return symbol;
   
} 
void edge_detection(unsigned char* in)
{
	int x, y;
	int gx;
	int gy;
	x_offset = 105;//14*7 + 7;
	skip_offset = 7;
	unsigned char sobel_input[9][9]; // new because we add one row, one column
	unsigned char* base = (unsigned char*)(ASCII_ADDR);
	base += x_offset;
	
	in = in + 119;//16*7 + 7;
	// copy the new input
	unsigned int i, j ;
	for(j = 0; j < 9; j++){
    for(i = 0; i < 9; i++){
        sobel_input[i][j] = *in++;
    }
    in = in + 7;
    }
	
	for(y = 1; y < 8; y++){
	    for(x = 1; x < 8; x++){
	        gx = (-sobel_input[x-1][y-1] + sobel_input[x-1][y+1]) 
	            + ( -(sobel_input[x][y-1]<<1) +(sobel_input[x][y+1]<<1) ) + 
	        ( -sobel_input[x+1][y-1] +sobel_input[x+1][y+1] );
		
	        gy = (sobel_input[x-1][y-1] + (sobel_input[x-1][y]<<1) + sobel_input[x-1][y+1] )+
	                        (-(sobel_input[x+1][y-1]) -  (sobel_input[x+1][y]<<1) -sobel_input[x+1][y+1] );
            

           if(gx < 0) gx = ~gx + 1;
           if(gy < 0) gy = ~gy + 1;
   	      // *base++ = ascii_art(gx*gx + gy*gy);
  	       *base++ = ascii_art(gx + gy);
	    }
	    base += skip_offset;
	}
	
	
}
int main(void) {



  //  delay(2);
    alt_mutex_dev* mutex_0 = altera_avalon_mutex_open( "/dev/mutex_0" );
    alt_mutex_dev* mutex_1 = altera_avalon_mutex_open( "/dev/mutex_1" );
    int mask = 8; // cpu_4's mask is 2^4
    
    while(1){
   
        /* wait for a picture to be in shared */

        while(!(*flag_rgb > 0));
        // update flag
        altera_avalon_mutex_lock(mutex_0, 1);
        *flag_rgb -= 1;
        altera_avalon_mutex_unlock(mutex_0);
        
        // read
        shared2_chip_gray((unsigned char *) RGB_ADDR, input_matrix);

        // resizd and write back
        interpolation(input_matrix, (unsigned char *) RESIZED_ADDR);
        /* set flag finish inter*/
        *flag_inter_4 = 1;
        /* wait for everyone to put picture back */
        while(!( *flag_inter_1 == 1 && *flag_inter_2 == 1 && *flag_inter_3 == 1 ));
        
        // wait for last picture to be consumed
		while( (*flag_ascii & mask ) != 0);
        edge_detection( (unsigned char *)RESIZED_ADDR);
        *flag_inter_4 = 0;
        /* set flag finish sobel */
        altera_avalon_mutex_lock(mutex_1, 1);
        *flag_ascii |= mask;
        altera_avalon_mutex_unlock(mutex_1);
        
       // delay(10000);
   
    }
    return 0;
}
