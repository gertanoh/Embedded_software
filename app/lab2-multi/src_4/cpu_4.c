#include <stdio.h>
#include "system.h"
#include "altera_avalon_mutex.h"


#define TRUE 1
extern void delay (int millisec);

unsigned char sqrtImproved(int x);
/*
void graycolor ()
{
    int x = 0 ,y = 0;
    unsigned char*  gray = (unsigned char*) SHARED_ONCHIP_BASE;
    int size_x= *gray++;
    int size_y = *gray++;
    gray++;
    //cpu_3 works on a quarter of the memory from x/2 to x and y/2 to y
    gray += (size_y>>1)*size_x + (size_x >> 1);
    size_x = size_x >> 1;
    size_y = size_y >> 1;
    unsigned char* alpha = (unsigned char*) SHARED_ONCHIP_BASE;
    double L = 0 ;
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
}*/
/*
void resize()
{
   int x = 0 ,y = 0; //Henry
    unsigned char*  gray = (unsigned char*) SHARED_ONCHIP_BASE;
    int size_x= *gray++;
    int size_y = *gray++;
    gray++;
    //cpu_2 works on a quarter of the memory from x/2 to x and y/2 to y
    gray += (size_y>>1)*size_x + (size_x >> 1);
    size_x = size_x >> 1;
    size_y = size_y >> 1;
    unsigned char* resize_imaged ;
    resize_imaged = gray;
    
    size_x = size_x >> 1;
    size_y = size_y >> 1;
    
    for(y = 0; y < size_y  ; y++)
        for( x = 0; x < size_x; x++){
            *resize_imaged++ = ( *gray + *(gray + 1) + *(gray + size_x)  + *(gray + size_x  + 1) ) / 4 ;
            gray = gray + 2;
        }
}*/
void edge_detection()
{
    int x = 0  ,y = 0;
    unsigned char*  pixel_pointer = (unsigned char*) SHARED_ONCHIP_BASE;

 
    
    int size_x= (*pixel_pointer++);
    int size_y = (*pixel_pointer++);
    pixel_pointer++;
    
    size_x = size_x >> 1;
    size_y = size_y >> 1;
     /*cpu_2 works on a quarter of the memory from x/2 to x and y/2 to y*/
    pixel_pointer += (size_y>>1)*size_x + (size_x >> 1);
    size_x = size_x >> 1;
    size_y = size_y >> 1;
    // we put the data at the end of the old so no overwriting
    unsigned char* edge_value = (unsigned char*) ( pixel_pointer + ((size_x)) * ((size_y)) );    
    /* kernels */
    
        
    int tmp_x;
    int tmp_y; 
    for( y = 0; y < size_y - 2; y++)
        for( x = 0; x < size_x - 2; x++){
            // apply the sobel operator on each pixel
            tmp_x = (-(*pixel_pointer)) + ((*(pixel_pointer + 2)))+
            (-2 * (*(pixel_pointer + size_x))) + (2 * (*(pixel_pointer + 2 + size_x)))+
            (-(*(pixel_pointer + (2*size_x) ) ))  + ((*(pixel_pointer + 2 + (2*size_x))));
            
            tmp_y = (-(*pixel_pointer)) + (-2 * (*(pixel_pointer + 1))) + (-(*(pixel_pointer + 2)))+
            ((*(pixel_pointer + 2 * size_x))) + (2 * (*(pixel_pointer + 1+ 2 * size_x))) +  *(pixel_pointer + 2 + (2*size_x) );

            *edge_value++ = sqrtImproved(tmp_x * tmp_x + tmp_y * tmp_y );
            //edge_value++;
            pixel_pointer++;
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
    
    int x = 0  ,y = 0;  
    unsigned char*  pixel_pointer = (unsigned char*) SHARED_ONCHIP_BASE;

    // The picture is now half sized
    int size_x= (*pixel_pointer++);
    int size_y = (*pixel_pointer++);
    pixel_pointer++;
    /*cpu_2 works on a quarter of the memory from x/2 to x and y/2 to y*/
    pixel_pointer += size_x * (size_y>>1) + (size_x >> 1);
    size_x = size_x >> 1;
    size_y = size_y >> 1;
    
    /* mnew size of the pciture */
    size_x = size_x >> 1;
    size_y = size_y >> 1;
    
    unsigned char* toshared;
    toshared = pixel_pointer;
    //char symbols[] = {' ','-','.','_', ':', '=', '+','*', 'i','x','#','$','g','&','%','@' };
    char symbols[] =  {'@','%','&','m','$','o','x','i','*','+', '=', ':', '_', '.', '-',' ' };
    
            
    for( y = 0; y < size_y; y++){
        for( x = 0; x < size_x; x++){
            char symbol = '\0';
                int saturation = (int) ( ( (*pixel_pointer++)/255.0 )* 15 );
                symbol = symbols[saturation];
                *toshared++ = symbol;  
        }
        
    }
}
int main(void) {


	alt_mutex_dev* mutex_4 = altera_avalon_mutex_open(MUTEX_4_NAME);
	while (1) {
	
        altera_avalon_mutex_lock(mutex_4,1);
	    //graycolor();
        //resize();
        //edge_detection();	
        image_to_ascii();
        altera_avalon_mutex_unlock(mutex_4);
        //printf("unlock 3\n");
                delay(2);
        /*small sleep to enable the cpu 0 to work */
            
      } 
     
  return 0;
}
