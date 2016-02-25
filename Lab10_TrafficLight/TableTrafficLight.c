// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortBEF_Init(void);
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);

// Linked data structure
struct State {
	unsigned long out;
	unsigned long outPF;
	unsigned long delay;
	unsigned long next[8];
};
typedef const struct State StateType;
typedef StateType *StatePtr;
#define goW   0
#define waitW 1
#define goS   2
#define waitS 3
#define walk  4
#define notWalk  5
#define walkOff  6
#define notWalk2  7
#define walkOff2  8
StateType fsm[12]={
 {0x0C, 0x02, 100,{     goW,     goW,   waitW,   waitW,   waitW,   waitW,   waitW,  waitW}},  //goW
 {0x14, 0x02, 60, {   waitW,     goW,     goS,     goS,    walk,    walk,     goS,    goS}}, //waitW
 {0x21, 0x02, 100,{     goS,   waitS,     goS,   waitS,   waitS,   waitS,   waitS,  waitS}},  //goS
 {0x22, 0x02, 60, {   waitS,     goW,     goS,     goW,    walk,    walk,    walk,   walk}}, //waitS
 {0x24, 0x08, 100,{    walk, notWalk, notWalk, notWalk,    walk, notWalk, notWalk,notWalk}},  //walk
 {0x24, 0x08, 60, { notWalk, walkOff, walkOff, walkOff, walkOff, walkOff, walkOff,walkOff}},  //notWalk
 {0x24, 0x00, 60, {walkOff,notWalk2,notWalk2,notWalk2,notWalk2,notWalk2,notWalk2,notWalk2}},  //walkOff
 {0x24, 0x08, 60, {notWalk2,walkOff2,walkOff2,walkOff2,walkOff2,walkOff2,walkOff2,walkOff2}},  //notWalk2
 {0x24, 0x00, 60, {walkOff2,     goW,     goS,     goW,    walk,     goW,     goS,     goW}}};  //walkOff2

unsigned long S;
unsigned long Input;

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	
	SysTick_Init();
	
	PortBEF_Init();
	
	S=goW;
  
  EnableInterrupts();
  while(1){
		GPIO_PORTB_DATA_R = fsm[S].out;  // set lights
		GPIO_PORTF_DATA_R = fsm[S].outPF;
    SysTick_Wait10ms(fsm[S].delay);
    S = fsm[S].next[GPIO_PORTE_DATA_R]; 
  }
}

void PortBEF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x32;      			 // 1) B E F clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize
		// Port B
  GPIO_PORTB_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTB_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTB_DIR_R &= ~0x00;         // 4.1) no input,
  GPIO_PORTB_DIR_R |= 0x3F;          // 4.2) PB0-5 output  
  GPIO_PORTB_AFSEL_R &= 0x00;        // 5) no alternate function      
  GPIO_PORTB_DEN_R |= 0x3F;          // 7) enable digital pins PF0-5 
		// Port E
  GPIO_PORTE_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x07;         // 4.1) PE0-2 input,
  GPIO_PORTE_DIR_R |= 0x00;          // 4.2) no output  
  GPIO_PORTE_AFSEL_R &= 0x00;        // 5) no alternate function      
  GPIO_PORTE_DEN_R |= 0x07;          // 7) enable digital pins PE0-2
		// Port F
  GPIO_PORTF_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTF_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R &= ~0x00;         // 4.1) no input,
  GPIO_PORTF_DIR_R |= 0x0A;          // 4.2) PF1,3 output  
  GPIO_PORTF_AFSEL_R &= 0x00;        // 5) no alternate function      
  GPIO_PORTF_DEN_R |= 0x0A;          // 7) enable digital pins PF1,3 
}


void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}

// 800000*12.5ns equals 10ms
void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}
