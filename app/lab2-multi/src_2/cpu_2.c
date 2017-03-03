
#include <stdio.h>
#include "system.h"
#include "altera_avalon_mutex.h"



#define SIZE_X 32
#define SIZE_Y 32
#define PICTURE_OFFSET 16
#define READ_OFFSET 4000
#define SIZE_X_Q 8
#define SIZE_Y_Q 8


/* set sof
 * flags for synchronisation 
 */

volatile unsigned char* flag_finish_sram = (volatile unsigned char*)SHARED_ONCHIP_BASE;
 
volatile unsigned char* flag_finish_sobel_2 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+2);

volatile unsigned char* flag_finish_interpolation_1 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+5);
volatile unsigned char* flag_finish_interpolation_2 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+6);
volatile unsigned char* flag_finish_interpolation_3 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+7);
volatile unsigned char* flag_finish_interpolation_4 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+8);

volatile unsigned char* flag_finish_read_2 = (volatile unsigned char*)(SHARED_ONCHIP_BASE+10);


unsigned char input_matrix[16][16];
unsigned char output_matrix[8][8];
unsigned char output_matrix_final[7][7];
int shared[12][16];
unsigned char edge_down[8][1];
unsigned char edge_left[1][9];


alt_mutex_dev *mutex_0;

void shared2_chip()
{
	int x, y;
	int* base = (int*)(SHARED_ONCHIP_BASE+PICTURE_OFFSET);
	//converting image to gray scale
	// write 4 bytes to the shared memory
	/* we use the avalon bus size to write 4 bytes in one shot
	* calculation of offset in order to get the right up 12 * 12
	*/
	unsigned char size_x_off = (SIZE_X * 3) >> 3;
	unsigned char size_y_off =  (SIZE_Y )>> 1 ;
	
	/* offset for cpu_2 to get the left upper quarter */
	base += size_x_off;
	for(y = 0; y < size_y_off ; y++){
	    for(x = 0; x < size_x_off  ; x++){
		    shared[x][y]= *(base++);
	    }
	    base += size_x_off;
	}

}

void grayscale(){
    int x, y;
    /* backward of the pointer in order to read */
    unsigned char* gray ;
    gray = shared; 
    int temp = 0;
    
	for(y = 0; y < 16; y++)
	    {
	    for(x = 0; x < 16; x++)
	    {
	        temp = (*gray)<<2; // R
	        temp += *gray++; // R
	        temp += (*gray)<<3; // G
	        temp += *gray++; // G
	        temp = temp >> 4;
	        temp += (*gray++)>> 3; //B
		    input_matrix[x][y] = temp;
	    }
	}
}
void interpolation()
{
	int x, y;
    int i, j;
	for(y = 0, j = 0; y < 15; y+=2,j++){
	    for(x = 0, i = 0; x < 15; x+=2, i++){
	    	   output_matrix[i][j] = (input_matrix[x][y] +input_matrix[x+1][y]
	    	   +input_matrix[x][y+1] + input_matrix[x+1][y+1] ) >> 2;
	    }
	}
}
void write_back_to_shared(){

    int x, y = 0;
    int* base = (int*)(SHARED_ONCHIP_BASE+READ_OFFSET);
    unsigned char size_x_off = 2;
	unsigned char size_y_off = 8 ;
	base += size_x_off;
	for(y = 0; y < size_y_off ; y++){
	    for(x = 0; x < size_x_off  ; x++){
		    *base++ = output_matrix[x][y];
	    }
	    base += 2;
	}

}
void get_edge(){
        
    unsigned char* base = (unsigned char*)(SHARED_ONCHIP_BASE+READ_OFFSET);
    /*position at the right upper */
    base += 7;
    int i = 0, j = 0;
    for(j= 0; j <9; j++){
        for(i = 0; i < 2; i++){
            edge_left[i][j] = *base ;
        }
        base += 16;
    }
        
    base = (unsigned char*)(SHARED_ONCHIP_BASE+READ_OFFSET);
    /*position at the right upper */
    base += 16*8 + 8;
    for(j= 0; j <2; j++){
        for(i = 0; i < 8; i++){
            edge_down[i][j] = *base++ ;
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
void edge_detection()
{
	int x, y;
	int gx[7][7];
	int gy[7][7];
	
	/* add left column */
	unsigned char temp_array[9][9];
	for(y = 0; y < 9; y++){
	    for(x = 0; x < 1; x++){
	        temp_array[x][y] = edge_left[x][y];
        }
    }
    /* add lower colum */
    for(y = 8; y < 9; y++){
	    for(x = 0; x < 8; x++){
	        temp_array[x+1][y] = edge_down[x][y];
        }
    }
    /* add out_matrix */
    for(y = 0; y < 8; y++){
	    for(x = 0; x < 8; x++){   
            temp_array[x+1][y+1] = output_matrix[x][y];
        }
    }
    
    	
	for(y = 1; y < SIZE_Y_Q; y++){
	    for(x = 1; x < SIZE_X_Q; x++){
	        gx[x-1][y-1] = (-temp_array[x-1][y-1] + temp_array[x-1][y+1]) 
	            + ( -(temp_array[x][y-1]<<1) +(temp_array[x][y+1]<<1) ) + 
	        ( -temp_array[x+1][y-1] +temp_array[x+1][y+1] );
		
	        gy[x-1][y-1] = (temp_array[x-1][y-1] + (temp_array[x-1][y]<<1) + temp_array[x-1][y+1] )+
	                        (-(temp_array[x+1][y-1]) -  (temp_array[x+1][y]<<1) -temp_array[x+1][y+1] );
            
	        //if(gx[x-1][y-1]< 0) gx[x-1][y-1]= -gx[x-1][y-1];
   	      // if(gy[x-1][y-1]< 0) gy[x-1][y-1]= -gy[x-1][y-1];
   	        output_matrix_final[x-1][y-1] = ascii_art(gx[x-1][y-1]*gx[x-1][y-1] + gy[x-1][y-1]*gy[x-1][y-1]);
	    }
	}
	
	unsigned char* base = (unsigned char*)(SHARED_ONCHIP_BASE+READ_OFFSET);
    unsigned char size_x_off = 6;
	unsigned char size_y_off = 6 ;
	base += size_x_off+1 ;
	for(y = 0; y < size_y_off  ; y++){
	    for(x = 0; x < size_x_off ; x++){
		    *base++ = output_matrix_final[x][y];
	    }
	    base += size_x_off+1;
	}
	
	
}


int main(void) {

   while(1){
   
       /* wait for a picture to be in shared */
        delay(1);
        while(!(*flag_finish_sram == 1));
        shared2_chip();
        *flag_finish_read_2 = 1;
        *flag_finish_sobel_2 = 0;
        *flag_finish_interpolation_2 = 0; 
        grayscale();
        interpolation();
        write_back_to_shared();
        /* set flag finish inter*/
        *flag_finish_interpolation_2 = 1;
        /* wait for everyone to put picture back */
        while(!( *flag_finish_interpolation_1 == 1 && *flag_finish_interpolation_3 == 1 && *flag_finish_interpolation_4 == 1 ));
        
        get_edge();
        edge_detection();
        /* set flag finish sobel */
        *flag_finish_sobel_2  = 1;
   
   }
  return 0;
}
