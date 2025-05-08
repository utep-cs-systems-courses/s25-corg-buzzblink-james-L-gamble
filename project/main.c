#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"
//#include "ledControl.h"




#define LED_RED BIT0               // P1.0
#define LED_GREEN BIT6             // P1.6
#define LEDS (BIT0 | BIT6)

#define SW1 BIT3		/* switch1 is p1.3 */
#define SWITCHES SW1            /* only 1 switch on this board */

#define S1 BIT0 /* S1 is p2.0 */
#define S2 BIT1 /* S2 is p2.1 */
#define S3 BIT2 /* S3 is p2.2 */
#define S4 BIT3 /* S4 is p2.3 */

int TOP_SWITCHES = 0x00001111;

extern int ledControl(int r, int g);

int playNote(short note){

  //The 5 buttons play the notes of an E minor pentatonic scale.
  switch(note){
    
  case(0):
    buzzer_set_period(329); //E3
    ledControl(0,0);
    break;
  case(1):
    buzzer_set_period(392); //G4
    ledControl(0,1);
    break;
  case(2):
    buzzer_set_period(440); //A4
    ledControl(1,0);
    break;
  case(3):
    buzzer_set_period(493); //B4
    ledControl(1,1);
    break;
  case(4):
    buzzer_set_period(587); //D5
    ledControl(0,0);
    break;
  }

  return 0;
}

void main(void) 
{  
  configureClocks();

  P1DIR |= LEDS;
  P1OUT &= ~LEDS;		/* leds initially off */
  
  P1REN |= SWITCHES;		/* enables resistors for switches */
  P1IE |= SWITCHES;		/* enable interrupts from switches */
  P1OUT |= SWITCHES;		/* pull-ups for switches */
  P1DIR &= ~SWITCHES;		/* set switches' bits for input */




  P2REN |= TOP_SWITCHES;
  P2IE |= TOP_SWITCHES;
  P2OUT |= TOP_SWITCHES;
  P2DIR &= ~TOP_SWITCHES;

  
  buzzer_init(); //Start speaker
  
  or_sr(0x18);  // CPU off, GIE on
} 

void
switch_interrupt_handler()
{
  char p1val = P1IN;		/* switch is in P1 */


/* update switch interrupt sense to detect changes from current buttons */
  P1IES |= (p1val & SWITCHES);	/* if switch up, sense down */
  P1IES &= (p1val | ~SWITCHES);	/* if switch down, sense up */

/* up=red, down=green */
  if (p1val & SW1) {
    playNote(0); //Side button on main board plays an E
  }
  
}

void
switch_interrupt_handler_2()
{

  char p2val = P2IN;

  P2IES |= (p2val & TOP_SWITCHES);
  P2IES &= (p2val | ~TOP_SWITCHES);

  if(p2val & S1){
    playNote(1); //S1 plays a G
  }

  if(p2val & S2){
    playNote(2); //S2 plays an A
  }

  if(p2val & S3){
    playNote(3); //S3 plays a B
  }

  if(p2val & S4){
    playNote(4); //S4 plays a D
  }
}

/* Switch on P1 (S2) */
void
__interrupt_vec(PORT1_VECTOR) Port_1(){
  if (P1IFG & SWITCHES) {	      /* did a button cause this interrupt? */
    P1IFG &= ~SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}

/* Switch on P2  */
void
__interrupt_vec(PORT2_VECTOR) Port_2(){
  if (P2IFG & TOP_SWITCHES) {	      /* did a button cause this interrupt? */
    P2IFG &= ~TOP_SWITCHES;		      /* clear pending sw interrupts */
    switch_interrupt_handler();	/* single handler for all switches */
  }
}
