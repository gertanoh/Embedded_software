
#include <stdio.h>
#include "system.h"

#define DEBUG 1
#define SIZE_X 32
#define SIZE_Y 32
#define SECTION_1 1
#define PICTURE_OFFSET 12
#define READ_OFFSET 1040


#define SIZE_X_Q 8
#define SIZE_Y_Q 8


/* set sof
 * flags for synchronisation 
 */
volatile unsigned char* flag_finish_sram = (unsigned char*)SHARED_ONCHIP_BASE;
 
volatile unsigned char* flag_finish_sobel_4 = (unsigned char*)(SHARED_ONCHIP_BASE+4);

volatile unsigned char* flag_finish_interpolation_1 = (unsigned char*)(SHARED_ONCHIP_BASE+5);
volatile unsigned char* flag_finish_interpolation_2 = (unsigned char*)(SHARED_ONCHIP_BASE+6);
volatile unsigned char* flag_finish_interpolation_3 = (unsigned char*)(SHARED_ONCHIP_BASE+7);
volatile unsigned char* flag_finish_interpolation_4 = (unsigned char*)(SHARED_ONCHIP_BASE+8);

volatile unsigned char* flag_finish_read_4 = (unsigned char*)(SHARED_ONCHIP_BASE+12);


unsigned char input_matrix[16][16];
unsigned char output_matrix[8][8];
unsigned char output_matrix_final[7][7];
int shared[12][16];

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
	unsigned char size_y_off =  (SIZE_X )>> 1 ;
	
	/* offset for cpu_3 to get the right down quarter */
	base += ((size_x_off<<1) * size_y_off);
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

    int* base = (int*)(SHARED_ONCHIP_BASE+READ_OFFSET);
    unsigned char size_x_off = 8;
	unsigned char size_y_off = 8 ;
	base += size_x_off * size_y_off;
	base += size_x_off;
	for(y = 0; y < size_y_off ; y++){
	    for(x = 0; x < size_x_off  ; x++){
		   *base++ = output_matrix[x][y];
	    }
	    base += size_x_off;
	}
}
    
void edge_detection()
{
	int x, y;
	int gx[7][7];
	int gy[7][7];
	
	
	for(y = 1; y < SIZE_Y_Q-1; y++){
	    for(x = 1; x < SIZE_X_Q-1; x++){
	        gx[x-1][y-1] = (-output_matrix[x-1][y-1] + output_matrix[x-1][y+1]) 
	            + ( -(output_matrix[x][y-1]<<1) +(output_matrix[x][y+1]<<1) ) + 
	        ( -output_matrix[x+1][y-1] +output_matrix[x+1][y+1] );
		
	        gy[x-1][y-1] = (output_matrix[x-1][y-1] + (output_matrix[x-1][y]<<1) + output_matrix[x-1][y+1] )+
	                        (-(output_matrix[x+1][y-1]) -  (output_matrix[x+1][y]<<1) -output_matrix[x+1][y+1] );
            
	       if(gx[x-1][y-1]< 0) gx[x-1][y-1]= -gx[x-1][y-1];
   	       if(gy[x-1][y-1]< 0) gy[x-1][y-1]= -gy[x-1][y-1];
   	        output_matrix_final[x-1][y-1] = ascii_art(gx[x-1][y-1] + gy[x-1][y-1]);
	    }
	}
	
	
}

void write_back_shared(){

    unsigned* base = (unsigned*)(SHARED_ONCHIP_BASE+READ_OFFSET);
    unsigned char size_x_off = 8;
	unsigned char size_y_off = 8 ;
	base += 16 * 8;
	for(y = 0; y < size_y_off -1 ; y++){
	    for(x = 0; x < size_x_off -1 ; x++){
		    *base++ = output_matrix_final[x][y];
	    }
	    base += size_x_off;
	}
}
int main(void) {
   
   
   int i = 0;
   while(i++ < 5){
   
        /* wait fora picture to be in shared */
        while(!(*flag_finish_sram == 1));
        /*first reset flag */
        printf("read to chip ");
        shared2_chip();
        /* set flag finsih reading */
        *flag_finish_read_4 = 1;
        grayscale();
        printf("gray\n");
        interpolation();
        printf("inter\n");
        write_back_to_shared();
        printf("write back to shared \n");
        /* set flag finish inter*/
        *flag_finish_interpolation_4 = 1;
        /* wait for everyone to put picture back */
        while(!( *flag_finish_interpolation_2 == 1 && *flag_finish_interpolation_3 == 1 && *flag_finish_interpolation_1 == 1 ));
        printf("Get the edge \n");
        printf("SObel \n");
        /* set flag finish sobel */
        *flag_finish_sobel_4 = 1;
   
   }
  return 0;
}
