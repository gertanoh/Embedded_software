


/* Code for 
* SIngle RTOS solution 
*/
#include <stdio.h>
#include "altera_avalon_performance_counter.h"
#include "includes.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "sys/alt_alarm.h"
#include "system.h"
#include "io.h"
#include "images.h"

#define DEBUG 0

#define HW_TIMER_PERIOD 100 /* 100ms */

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

extern void delay (int millisec);



void sram2sm_p3(int* base)
{
	int x, y;
	unsigned char size_x = (unsigned char*)*base;
	unsigned char size_y = ( (*base) >> 8 );
	int size ;
	size =  ( (size_x * size_y *3 ) >> 2) + 1;
	    
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
    unsigned char* gray = (unsigned char*) SHARED_ONCHIP_BASE;
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
 /*
void interpolation()
{
	int x, y;
	unsigned char* shared;
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	int SIZE_X = *shared++;
	int SIZE_Y = *shared++;
	shared++;
	int new_size = SIZE_X >>1;
		
	int  ax= 0, ay = 0;
	for(y = 0; y < (SIZE_Y-1); y+=2){
	    ax = 0;
	    for(x = 0; x < (SIZE_X-1); x+=2){
	    	    
		    *(shared+(ay*new_size)+ax) = ( *(shared+(y*SIZE_X)+x)  +  *(shared+(y*SIZE_X)+(x+1) )  
		                    + *(shared+((y+1)*SIZE_X)+(x) ) + *(shared+((y+1)*SIZE_X)+(x+1) ) )   >> 2;
		    
		    ax++;
	    }
	    ay++;
	}
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	*shared++ = SIZE_X >> 1;
	*shared++ = SIZE_Y >> 1;
	
}*/
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
 int min = 0, max = 0;
 /*
void edge_detection()
{
	int x, y;
	unsigned char* shared;
	int gx,gy;
	shared = (unsigned char*) SHARED_ONCHIP_BASE;
	int SIZE_X = *shared++;
	int SIZE_Y = *shared++;
	shared++;

	int maxim=0;
	int i = 0;
	for(y = 1; y < SIZE_Y-1; y++){
	    for(x = 1; x < SIZE_X-1; x++){
	        gx=(-(*(shared+(x-1)+((y-1)*SIZE_X))) + 
		        (*(shared+(x+1)+((y-1)*SIZE_X))) + 
		        -( (*(shared+(x-1)+((y)*SIZE_X)))<< 1 ) + 
		        ( (*(shared+(x+1)+((y)*SIZE_X)))<< 1 ) + 
		        (-(*(shared+(x-1)+((y+1)*SIZE_X)))) + 
		        (*(shared+(x+1)+((y+1)*SIZE_X))) );
		
	        gy=(-(*(shared+(x-1)+((y-1)*SIZE_X))) + 
		        -( (*(shared+(x)+((y-1)*SIZE_X))) << 1 ) + 
		        (-(*(shared+(x+1)+((y-1)*SIZE_X)))) + 
		        (*(shared+(x-1)+((y+1)*SIZE_X))) + 
		        ( (*(shared+(x)+((y+1)*SIZE_X))) << 1) + 
		        (*(shared+(x+1)+((y+1)*SIZE_X)))) ;
            
	       
	        //maxim= (abs(gxval)+abs(gyval) )  ;
	        maxim = sqrt(gx*gx + gx*gy)*2/11;
	       
	        *(shared+(y*SIZE_X)+x) = maxim;
	    }
	}
}
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
	                        gx[x-1][y-1] = (-input_matrix[x-1][y-1] + input_matrix[x-1][y+1]) 
	            + ( -(input_matrix[x][y-1]<<1) +(input_matrix[x][y+1]<<1) ) + 
	        ( -input_matrix[x+1][y-1] +input_matrix[x+1][y+1] );
		
	        /*gy[x-1][y-1] = (input_matrix[x-1][y-1] + (input_matrix[x-1][y]<<1) + input_matrix[x-1][y+1] )+
	                        (-(input_matrix[x+1][y-1]) -  (input_matrix[x+1][y]<<1) -input_matrix[x+1][y+1] );*/
            
	       //if(gx[x-1][y-1]< 0) gx[x-1][y-1]= -gx[x-1][y-1];
   	       //if(gy[x-1][y-1]< 0) gy[x-1][y-1]= -gy[x-1][y-1];
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


void ascii_art()
{
	unsigned char* base;
	//char symbols[16] = { '{','y','w','@', '%','&', '#', 'x','`','_', '+', '=', ':', '-', '.', ' ' };
    //char symbols[16] =  {'@','%','#','&','m','$','o','x','i','*','+', '=', ':', '.', '-',' ' };
    char symbols[] = {' ','-','.','_', ':', '=', '+','*', 'i','x','#','$','g','&','%','@' };
	base = (unsigned char*) SHARED_ONCHIP_BASE;
    int SIZE_X = *base++;
    int SIZE_Y = *base++;
    *base++;
	int j,y,x;	
	for(y = 1; y < SIZE_Y-1; y++)
	{
		for(x = 1; x < SIZE_X-1; x++)
		{
			j= (int) (((*(base+ (y*SIZE_X)+x)) )>>4);
			*(base++) = symbols[j];
		}
	}
}
void write_sram(unsigned char *base){
    int x, y;
	unsigned char* shared;
	shared = (unsigned char*) (SHARED_ONCHIP_BASE);
	int SIZE_X = *shared++;
	int SIZE_Y = *shared++;
	shared++;
	for(y = 1; y < (SIZE_Y-1); y++){
		for(x = 1; x < (SIZE_X-1 ); x++)
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
	int i;
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


#if DEBUG == 1
    PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    PERF_START_MEASURING (PERFORMANCE_COUNTER_0_BASE);
    PERF_BEGIN(PERFORMANCE_COUNTER_0_BASE, SECTION_1);
   /* Measurement here */
    sram2sm_p3(img_array[current_image]);
    grayscale();
    interpolation();
    edge_detection();
	ascii_art();
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
            ascii_art();
            write_sram(out);
	        // Print report 
	        
            current_image=(current_image+1) % number_of_images; 
        }
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
