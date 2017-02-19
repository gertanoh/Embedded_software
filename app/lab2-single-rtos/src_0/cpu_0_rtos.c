// Skeleton for lab 2
// 
// Task 1 writes periodically RGB-images to the shared memory
//
// No guarantees provided - if bugs are detected, report them in the Issue tracker of the github repository!

#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 1

#define HW_TIMER_PERIOD 100 /* 100ms */
extern void delay (int millisec);   
/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    StartTask_Stack[TASK_STACKSIZE]; 

/* Definition of Task Priorities */ 

#define STARTTASK_PRIO      1
#define TASK1_PRIORITY      10

/* Definition of Task Periods (ms) */
#define TASK1_PERIOD 10000

#define SECTION_1 1
#define SECTION_2 2
#define SECTION_3 3
#define SECTION_4 4
#define SECTION_5 5

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
	int max_col= *base++;
	*shared++  = size_x;
	*shared++  = size_y;
	*shared++  = max_col;
	printf("The image is: %d x %d!! \n", size_x, size_y);
	for(y = 0; y < size_y; y++)
	for(x = 0; x < size_x; x++)
	{
		*shared++ = *base++; 	// R
        *shared++ = *base++;	// G
		*shared++ = *base++;	// B
		//printf("The grayscale is: %d \n " , Y);

	   
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
   // *Y = *(max_col);
    for(y = 0; y < size_y; y++)
	    for(x = 0; x < size_x; x++)
	    {
	        double red = 0.3125;
	        double green = 0.5625;
	        double blue = 0.125;
	        L = red * *gray++ + green * *gray++ + blue * *gray++;
	        *alpha++ = L;
            //gray = (unsigned char*) SHARED_ONCHIP_BASE;
            //printf("The grayscale is: %d \n " , *L);
            //gray++;
           // *
        }
}
void resize()
{
   /*int x ,y;
    unsigned char*  gray = (unsigned char*) SHARED_ONCHIP_BASE;
    
    int short size_x= *gray++;
    int short size_y = *gray++;
    int short max_col= *gray++;
   // unsigned char;
   //unsigned char* L = *gray;
   unsigned char Y[y][x];
       Y[y][x]= (*gray++);
    //*Y=gray;
    for(y = 0; y < size_y; y++)
	    for(x = 0; x < size_x; x++)
	    {
	         //*L = 0.3125 * *gray++ + 0.5625 * *gray++ + 0.125 * *gray++;
            //gray = (unsigned char*) SHARED_ONCHIP_BASE;
            //R [0][0] = (Y[0][0]+Y[0][1]+Y[1][0]+Y[1][1])/4;
            //*Y[y][x]= *L;
            *gray++ = ((Y[y][x]) + (Y[y][x+1]) + (Y[y+1][x]) + (Y[y+1][x+1]))/4;
            //printf(" size %d\n ",Y[y][x]);
            
        }*/
       /* printf("\n");
        *(SHARED_ONCHIP_BASE) = *SHARED_ONCHIP_BASE)/2;
        
        *(SHARED_ONCHIP_BASE+1)= *(SHARED_ONCHIP_BASE+1)/2;*/
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

    // The picture is now half sized (shouldnt divide the pointer)
   /* int size_x= (*pixel_pointer++)/2;
    int size_y = (*pixel_pointer++)/2;
    int max_col = *pixel_pointer++;*/
    
    int size_x= (*pixel_pointer++);
    int size_y = (*pixel_pointer++);
    int max_col =( *pixel_pointer++);
    
    // we put the data at the end of the old so no overwriting
    unsigned char* edge_value = (unsigned char*) ( pixel_pointer + ((size_x)/2) * ((size_y)/2) );    
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
        
        
    for( y = 0; y < size_y/2; y++)
        for( x = 0; x < size_x/2; x++){
            // apply the sobel operator on each pixel
            int tmp_x = (X[0][0] * (*pixel_pointer)) + (X[0][1] * (*(pixel_pointer + 1))) + (X[0][2] * (*(pixel_pointer + 2)))+
        (X[0][0] * (*(pixel_pointer + size_x/2))) + (X[0][1] * (*(pixel_pointer + 1+ size_x/2))) + (X[0][2] * (*(pixel_pointer + 2 + size_x/2)))+
(X[0][0] * (*(pixel_pointer + 2 * size_x/2))) + (X[0][1] * (*(pixel_pointer + 1+ 2 * size_x/2))) + (X[0][2] * (*(pixel_pointer + 2 + 2 * size_x/2)));
            
            int tmp_y = (Y[0][0] * (*pixel_pointer)) + (Y[0][1] * (*(pixel_pointer + 1))) + (Y[0][2] * (*(pixel_pointer + 2)))+
        (Y[0][0] * (*(pixel_pointer + size_x/2))) + (Y[0][1] * (*(pixel_pointer + 1+ size_x/2))) + (Y[0][2] * (*(pixel_pointer + 2 + size_x/2)))+
(Y[0][0] * (*(pixel_pointer + 2 * size_x/2))) + (Y[0][1] * (*(pixel_pointer + 1+ 2 * size_x/2))) + (Y[0][2] * (*(pixel_pointer + 2 + 2 * size_x/2)));

            int tmp = tmp_x * tmp_x + tmp_y * tmp_y ;
            *edge_value = sqrtImproved(tmp);
            edge_value++;
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
    
    int x ,y;  
    unsigned char*  pixel_pointer = (unsigned char*) SHARED_ONCHIP_BASE;

    // The picture is now half sized
    int size_x= (*pixel_pointer++);
    int size_y = (*pixel_pointer++);
    int max_col = *pixel_pointer++;
    unsigned char* toshared;
    toshared= pixel_pointer;
    // the new data are after the old data
    //pixel_pointer += size_x * size_y ; 
    //char symbols[] = {' ','-','.','_', ':', '=', '+','*', 'i','x','#','$','g','&','%','@' };
  char symbols[] =  {'@','%','&','g','$','#','x','i','*','+', '=', ':', '_', '.', '-',' ' };
    
            
    for( y = 0; y < size_y; y++){
        for( x = 0; x < size_x; x++){
            char symbol = '\0';
          // char  symbol;
           // if( *pixel_pointer++ == 0){
                // symbol = '_';
           // }
            //else {
                //double red = 0.299;
                //double green = 0.587;
                //double blue = 0.114;
                /*double greyShade = *pixel_pointer++ * 0.299 +
                                    *pixel_pointer++ * 0.587 +
                                    *pixel_pointer++ * 0.114;*/
                int saturation = (int) ( ( (*pixel_pointer++)/255.0 )* 15 );
                symbol = symbols[saturation];
                *toshared++ = symbol;  
              // symbol = symbols[*pixel_pointer++];
               //pixel_pointer++;
            //}
           // printf("%c",symbol);
        }
        
        //line finshed
       // printf("\n");
    }
}
void writeback()
{
    int x ,y;  
    unsigned char*  pixel_pointer = (unsigned char*) SHARED_ONCHIP_BASE;

    // The picture is now half sized
    int size_x= (*pixel_pointer++);
    int size_y = (*pixel_pointer++);
    int max_col = *pixel_pointer++;

    unsigned char writeback [size_x][size_y];
    for(y = 0; y <size_y; y++){
        for(x = 0; x < size_x; x++){
            writeback[x][y] = *pixel_pointer++;
         //   printf("%c",writeback[x][y]);
        }
       // printf("\n");
    }

}
/*
 * Global variables
 */
int delay_timer; // Delay of HW-timer 

/*
 * ISR for HW Timer
 */
alt_u32 alarm_handler(void* context)
{
  OSTmrSignal(); /* Signals a 'tick' to the SW timers */
  
  return delay_timer;
}

// Semaphores
OS_EVENT *Task1TmrSem;

// SW-Timer
OS_TMR *Task1Tmr;

/* Timer Callback Functions */ 
void Task1TmrCallback (void *ptmr, void *callback_arg){
  OSSemPost(Task1TmrSem);
}

void task1(void* pdata)
{
	INT8U err;
	INT8U value=0;
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
    		    PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
    		
		        	//	PERF_RESET(PERFORMANCE_COUNTER_0_BASE);

    		//PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
		    graycolor();
		    PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_1); 
		    /*    		PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);*/
    		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_2);
	        resize();
    	    PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_2);
    	        	/*	PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE); */
    		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_3);
	        edge_detection();	
		    PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_3); 
		        	/*	PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);*/
    		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_4);
            image_to_ascii();
		    PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_4); 
		        		/*PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    		PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);*/
    		PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_5);
            writeback();
    	    PERF_END(PERFORMANCE_COUNTER_0_BASE, SECTION_5); 
            

		    /* Print report */
		    perf_print_formatted_report
		    (PERFORMANCE_COUNTER_0_BASE,            
		    ALT_CPU_FREQ,        // defined in "system.h"
		    5,                   // How many sections to print
		    "Section 1",
	        "section 2",
	        "Section 3",
	        "Section 4",
	        "Section 5"        // Display-name of section(s).
		    );   

		   	
		    /* delay */
		    delay(3000);
	    
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
  	   
		/* Just to see that the task compiles correctly */
		IOWR_ALTERA_AVALON_PIO_DATA(LEDS_GREEN_BASE,value++);

		OSSemPend(Task1TmrSem, 0, &err);

		/* Increment the image pointer */
		current_image=(current_image+1) % number_of_images;
    } 


	
}

void StartTask(void* pdata)
{
  INT8U err;
  void* context;

  static alt_alarm alarm;     /* Is needed for timer ISR function */
  
  /* Base resolution for SW timer : HW_TIMER_PERIOD ms */
  delay_timer = alt_ticks_per_second() * HW_TIMER_PERIOD / 1000; 
  printf("delay in ticks %d\n", delay_timer);

  /* 
   * Create Hardware Timer with a period of 'delay' 
   */
  if (alt_alarm_start (&alarm,
      delay_timer,
      alarm_handler,
      context) < 0)
      {
          printf("No system clock available!n");
      }

  /* 
   * Create and start Software Timer 
   */

   //Create Task1 Timer
   Task1Tmr = OSTmrCreate(0, //delay
                            TASK1_PERIOD/HW_TIMER_PERIOD, //period
                            OS_TMR_OPT_PERIODIC,
                            Task1TmrCallback, //OS_TMR_CALLBACK
                            (void *)0,
                            "Task1Tmr",
                            &err);
                            
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if creation successful
      printf("Task1Tmr created\n");
    }
   }
   

   /*
    * Start timers
    */
   
   //start Task1 Timer
   OSTmrStart(Task1Tmr, &err);
   
   if (DEBUG) {
    if (err == OS_ERR_NONE) { //if start successful
      printf("Task1Tmr started\n");
    }
   }


   /*
   * Creation of Kernel Objects
   */

  Task1TmrSem = OSSemCreate(0);   

  /*
   * Create statistics task
   */

  OSStatInit();

  /* 
   * Creating Tasks in the system 
   */

  err=OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  if (DEBUG) {
     if (err == OS_ERR_NONE) { //if start successful
      printf("Task1 created\n");
    }
   }  

  printf("All Tasks and Kernel Objects generated!\n");

  /* Task deletes itself */

  OSTaskDel(OS_PRIO_SELF);
}


int main(void) {

  printf("MicroC/OS-II-Vesion: %1.2f\n", (double) OSVersion()/100.0);
     
  OSTaskCreateExt(
	 StartTask, // Pointer to task code
         NULL,      // Pointer to argument that is
                    // passed to task
         (void *)&StartTask_Stack[TASK_STACKSIZE-1], // Pointer to top
						     // of task stack 
         STARTTASK_PRIO,
         STARTTASK_PRIO,
         (void *)&StartTask_Stack[0],
         TASK_STACKSIZE,
         (void *) 0,  
         OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
         
  OSStart();
  
  return 0;
}
