#include  "app_cfg.h"
#include  <cpu_core.h>
#include  <os.h>
#include  "..\bsp\bsp.h"
#include  "..\bsp\bsp_led.h"
#include  "..\bsp\bsp_sys.h"
// SAR Addition
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

static  OS_STK       AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       ButtonMonitorStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       BlinkyStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       ButtonAlertStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK       DebuggingVarsStk[APP_CFG_TASK_START_STK_SIZE];
                                    LOCAL FUNCTION PROTOTYPES
*********************************************************************************
static  void  AppTaskCreate         (void);
static  void  AppTaskStart          (void       *p_arg);
static  void  ButtonMonitor         (void       *p_arg);
static  void  Blinky          			(void       *p_arg);
static  void  ButtonAlert           (void       *p_arg);
static  void  DebuggingVars         (void       *p_arg);
OS_FLAG_GRP *EventFlags; 
OS_EVENT *UartMutex; 

#define Blinky_flag 0x01
#define ButtonAlert_flag 0x02

int  main (void)
{
#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  err;
#endif

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR     cpu_err;
#endif

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_NameSet((CPU_CHAR *)"TM4C129XNCZAD",
                (CPU_ERR  *)&cpu_err);
#endif

    CPU_IntDis();       // Disable all interrupts.                              

    OSInit();           // Initialize "uC/OS-II, The Real-Time Kernel"          

    OSTaskCreateExt((void (*)(void *)) AppTaskStart, // Create the start task  
                    (void      *) 0,
                    (OS_STK    *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U      ) APP_CFG_TASK_START_PRIO,
                    (INT16U     ) APP_CFG_TASK_START_PRIO,
                    (OS_STK    *)&AppTaskStartStk[0],
                    (INT32U     ) APP_CFG_TASK_START_STK_SIZE,
                    (void      *) 0,
                    (INT16U     )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start", &err);
#endif								
		UartMutex = OSMutexCreate(1,&err); 
		EventFlags = OSFlagCreate(0x00, &err); 	
		OSStart();      //starts multi-tasking (gives controller to OS-II)       
    while (1) {
        ;
    }
}
/*$PAGE*/
/***********************************************************************************                                        
*  App_TaskStart()
* Description : Startup task example code.
* Arguments   : p_arg       Argument passed by 'OSTaskCreate()'.
* Returns     : none.
* Created by  : main().
* Notes       : (1) The first line of code is used to prevent a compiler warning because 'p_arg' is not used.  The compiler should not generate any code for this statement.
**********************************************************************************/
static  void  AppTaskStart (void *p_arg)
{
    //uint32_t sw2_status;
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;

   (void)p_arg;            
   (void)&p_arg;
    BSP_Init();               // Initialize BSP functions                      

    cpu_clk_freq = BSP_SysClkFreqGet();    // Determine SysTick reference freq
    cnts         = cpu_clk_freq          // Determine nbr SysTick increments   
                 / (CPU_INT32U)OS_TICKS_PER_SEC;
   OS_CPU_SysTickInit(cnts);
    CPU_Init();                          // Initialize the uC/CPU services    

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                // Determine CPU capacity   
#endif

    Mem_Init();

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();
#endif

//below is for blinking LEDs
		BSP_LED_Toggle(0);
		OSTimeDlyHMSM(0, 0, 0, 200);
		BSP_LED_Toggle(0);
		BSP_LED_Toggle(1);
		OSTimeDlyHMSM(0, 0, 0, 200);
		BSP_LED_Toggle(1);
		BSP_LED_Toggle(2);
		OSTimeDlyHMSM(0, 0, 0, 200);    
		BSP_LED_Toggle(2);

		OSTimeDlyHMSM(0, 0, 1, 0);   

		AppTaskCreate();    // Creates all the necessary application tasks
		while (DEF_ON) {

        OSTimeDlyHMSM(0, 0, 0, 100);					

    }
}
/************************************************************************************
*                                         AppTaskCreate()
* Description :  Create the application tasks.
* Argument(s) :  none.
* Return(s)   :  none.
* Caller(s)   :  AppTaskStart()
* Note(s)     :  none.
**********************************************************************************/
static  void  AppTaskCreate (void)
{
OSTaskCreate((void (*)(void *)) ButtonMonitor ,          
                    (void     *) 0,			 
                    (OS_STK   *)&ButtonMonitorStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U     ) 5 );  			 
                
OSTaskCreate((void (*)(void *)) Blinky ,        
                    (void           *) 0,		 
                    (OS_STK         *)&BlinkyStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) 6 );  
										
OSTaskCreate((void (*)(void *)) ButtonAlert ,      
                    (void       *) 0,	 
                    (OS_STK     *)&ButtonAlertStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U       ) 7 );  	 
										
OSTaskCreate((void (*)(void *)) DebuggingVars ,      
                    (void     *) 0,	 
                    (OS_STK   *)&DebuggingVarsStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U     ) 8 );  		 
}

static  void  ButtonMonitor  (void *p_arg)
{
	uint32_t sw2_status;
	uint32_t sw1_status;
	
	INT8U err;
	OS_FLAGS value; 
 	
	(void)p_arg;  
	
    while (1) 
    {   
	sw1_status = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
	sw2_status = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0); 
				
	if (sw1_status == 0) 
	{ 
		value = OSFlagPost(EventFlags, Blinky_flag, OS_FLAG_SET, &err); 
	}
	if (sw2_status == 0) 
	{	 
		value = OSFlagPost(EventFlags, ButtonAlert_flag, OS_FLAG_SET, &err);
	}
		OSTimeDlyHMSM(0, 0, 0, 100);  // keep the delay 
     }
}

static  void  Blinky  (void *p_arg)
{
       int blink_rate = 100;	
	OS_FLAGS value; 
	INT8U err1; 
  
	(void)p_arg;
	
    while (1) 
    {          
	BSP_LED_Toggle(1);
				
	value = OSFlagAccept(EventFlags, Blinky_flag, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, &err1); 				
  	if (value == Blinky_flag) 
	{
		blink_rate = 100 + ( rand() % 1000 ); 
	} 
        
	OSTimeDlyHMSM(0, 0, 0, blink_rate);
     }
}

static  void  ButtonAlert   (void *p_arg)
{
	INT8U err; 
	OS_FLAGS value; 
	
	(void)p_arg;
	
	//set the pin/post alerts 
    while (1) 
    {    
	value = OSFlagPend(EventFlags, ButtonAlert_flag, OS_FLAG_WAIT_SET_ALL + OS_FLAG_CONSUME, 0, &err);
	if (value == ButtonAlert_flag) 
	{ 
		OSMutexPend(UartMutex, 0, &err); 
		UARTprintf("\n\nButtonAlert task -> SW2 pressed ");   
		OSMutexPost(UartMutex); 
	}
	OSTimeDlyHMSM(0, 0, 0, 400);
     }}

static  void  DebuggingVars  (void *p_arg)
{
INT8U err; 
  	(void)p_arg;
	
	//set the pin/post alerts 
	
    while (1) 
    {   
	OSMutexPend(UartMutex, 0, &err); 	//check if its available then give it the task, if not, it waits			
	UARTprintf("\n\nOSCPUUsage ");
	UARTprintf("\nOSCtxSwCtr "); 
	UARTprintf("\nOSIdleCtr  "); 
	OSMutexPost(UartMutex); 
	OSTimeDlyHMSM(0, 0, 0, 900); 
	}
}
