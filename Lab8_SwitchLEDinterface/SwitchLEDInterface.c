// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// Constant declarations to access port registers using 
// symbolic names instead of addresses
#define GPIO_PORTE_DATA_R       (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R        (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R      (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R        (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_DEN_R        (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_LOCK_R       (*((volatile unsigned long *)0x40024520))
#define GPIO_PORTE_CR_R         (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R      (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R       (*((volatile unsigned long *)0x4002452C))
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void);
void Delay1ms(unsigned long msec);

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  PortE_Init();
	
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
		if (GPIO_PORTE_DATA_R&0x01) { // Switch pressed
			if (GPIO_PORTE_DATA_R&0x02) { // If led on
				GPIO_PORTE_DATA_R&=0xFD; // Switch led off
			} else {											// If led off
				GPIO_PORTE_DATA_R|=0x02; // Switch led on
			}
		} else {
			GPIO_PORTE_DATA_R|=0x02; // Switch led on
		}
		Delay1ms(100);
  }
  
}

void PortE_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x10;      			 // 1) E clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTE_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x01;         // 4.1) PE0 input,
  GPIO_PORTE_DIR_R |= 0x02;          // 4.2) PE1 output  
  GPIO_PORTE_AFSEL_R &= 0x00;        // 5) no alternate function      
  GPIO_PORTE_DEN_R |= 0x03;          // 7) enable digital pins PE1,0       
}

void Delay1ms(unsigned long msec){
	unsigned long i;
	while (msec > 0) {
		i = 13333;
		while (i > 0) {
			i--;
		}
		msec--;
	}
}
