// ADCTestMain.c
// Runs on TM4C123
// This program periodically samples ADC channel 0 and stores the
// result to a global variable that can be accessed with the JTAG
// debugger and viewed with the variable watch feature.
// Daniel Valvano
// September 5, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// center of X-ohm potentiometer connected to PE3/AIN0
// bottom of X-ohm potentiometer connected to ground
// top of X-ohm potentiometer connected to +3.3V 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../inc/ST7735.h"
#include "../../inc/SysTick.h"
#include "../../inc/ADCSWTrigger.h"
#include "../../inc/tm4c123gh6pm.h"
#include "../../inc/PLL.h"
#include "../../inc/LaunchPad.h"
#include "../../inc/CortexM.h"
#include "../../inc/Timer2A.h"
#include "../../inc/Timer3A.h"

#define SIZE 1000
#define PD1 (*((volatile uint32_t *)0x40007008))
#define Debug_Set()		(PD1 = 0x02)
#define Debug_Clear()	(PD1 = 0x00)
int32_t Debug_time[SIZE];
uint32_t Debug_adc[SIZE];
uint32_t Debug_diff[SIZE];
uint32_t adc_num[64];
uint32_t adc_count[64];
uint32_t n = 0;
uint32_t min;
uint32_t max;
uint32_t diff;
uint32_t time_prev;
uint32_t time_current;
uint32_t time_elapsed;

struct adc_freq {
	uint32_t freq;
	uint32_t adc_value;
};


void Debug_profile(uint32_t Debug_ADC_value){
	if(n < SIZE){ 
		time_current = TIMER1_TAR_R;
		time_elapsed = time_prev - time_current;
		time_prev = time_current;
		Debug_time[n] = TIMER1_TAR_R;
		Debug_adc[n] = Debug_ADC_value;
		n++;
	}
}

void Timer1_Init(void){ volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count 
  TIMER1_TAILR_R = 0xFFFFFFFF;  // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

volatile uint32_t FallingEdges = 0;
void EdgeCounterPortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}

void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
	if(FallingEdges>3){
		FallingEdges = 0;
	}
	//4x, 16x, 64x
	switch(FallingEdges){
		case 0:
			ST7735_SetCursor(0, 1);
			ST7735_OutString("1  point(s)");
			ADC0_SAC_R &= 0x0000;
			break;
		case 1:
			ST7735_SetCursor(0, 1);
				ST7735_OutString("4  point(s)");
			ADC0_SAC_R &= 0x0002;
			break;
		case 2:
			ST7735_SetCursor(0, 1);
			ST7735_OutString("16 point(s)");
			ADC0_SAC_R &= 0x0004;
			break;
		case 3:
			ST7735_SetCursor(0, 1);
			ST7735_OutString("64 point(s)");
			ADC0_SAC_R &= 0x0006;
			break;
		}
}

volatile uint32_t ADCvalue;
// This debug function initializes Timer0A to request interrupts
// at a 1000 Hz frequency.  It is similar to FreqMeasure.c.
void Timer0A_Init100HzInt(void){
  volatile uint32_t delay;
  DisableInterrupts();
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  TIMER0_TAILR_R = 79999;         // start value for 1000 Hz interrupts
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer0A_Handler(void){
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
  PF2 ^= 0x04;                   // profile
  PF2 ^= 0x04;                   // profile
  ADCvalue = ADC0_InSeq3();
	Debug_profile(ADCvalue);
  PF2 ^= 0x04;                   // profile
}

void UserTask(void){uint8_t i;
	i++;
}

void Graph(void){
	for(int i=0; i<SIZE-1; i++){
		for(int j=0; j < SIZE-i-1; j++){
			if(Debug_adc[j] > Debug_adc[j+1]){
				uint32_t temp = Debug_adc[j];
				Debug_adc[j] = Debug_adc[j+1];
				Debug_adc[j+1] = temp;
			}
		}
	}
	// Loop and variables to divide buffer into element numbers and counts
	int a = 0;
	uint32_t iter = 0;
	uint32_t curr_num = Debug_adc[0];
	uint32_t curr_num_count = 0;
	while(a < SIZE){
		if(Debug_adc[a] != curr_num){
			adc_num[iter] = curr_num;
			adc_count[iter] = curr_num_count;
			curr_num = Debug_adc[a];
			curr_num_count = 0;
			iter++;
		} else {
			curr_num_count++;
			a++;
		}
	}
	// Add last number and number count
	adc_num[iter] = curr_num;
	adc_count[iter] = curr_num_count;
	iter++;
	
	uint32_t padding = (128-iter*2)/2;
	ST7735_XYplotInit("Lab 2\rPMF Plot\r",-2500, 2500, -2500, 2500, ST7735_Color565(228,228,228)); // light grey
	for(int i=0; i<iter;i++){
		if(adc_count[i] > 128){
			ST7735_Line(padding+i*2,160-128,padding+i*2,160,ST7735_BLUE);
		} else {
			ST7735_Line(padding+i*2,160-adc_count[i],padding+i*2,160,ST7735_BLUE);
		}
	}
	n = 0;
}

int main(void){
  PLL_Init(Bus80MHz);                   // 80 MHz
  LaunchPad_Init();                     // activate port F
  ADC0_InitSWTriggerSeq3_Ch9();         // allow time to finish activating
  Timer0A_Init100HzInt();               // set up Timer0A for 1000 Hz interrupts
	Timer1_Init();
	Timer2A_Init(&UserTask,8000,1); // initialize timer2 (10kHz)
	PF2 = 0;                      // turn off LED
	ST7735_InitR(INITR_REDTAB);
	ST7735_FillScreen(ST7735_BLACK);
  ST7735_SetCursor(0,0);
	ST7735_XYplotInit("Lab 2 PMF, averaging\r",-2500, 2500, -2500, 2500, ST7735_Color565(228,228,228)); // light grey
  EnableInterrupts();
  while(n < SIZE){
    PF1 = (PF1*12345678)/1234567+0x02;  // this line causes jitter
  }
	
	min = Debug_time[0] - Debug_time[1];
	max = Debug_time[0] - Debug_time[1];
	for(int i=0; i<SIZE-1; i++){
		diff = abs(abs(Debug_time[i]) - abs(Debug_time[i+1]));
		Debug_diff[i] = diff;
		if(diff < min){
			min = diff;
		}else if(diff > max){
			max = diff;
		}
	}
	Timer3A_Init(&Graph,80000000,1); // initialize timer3 (1Hz)
	while(1){
		WaitForInterrupt();
	}
}
