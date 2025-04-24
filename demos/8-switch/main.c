#include <msp430.h>
#include "libTimer.h"

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
    P1OUT |= LED_RED;
    P1OUT &= ~LED_GREEN;
  } else {
    P1OUT |= LED_GREEN;
    P1OUT &= ~LED_RED;
  }
}

void
switch_interrupt_handler_2()
{

  char p2val = P2IN;

  P2IES |= (p2val & TOP_SWITCHES);
  P2IES &= (p2val | ~TOP_SWITCHES);

  if(p2val & S1){
    P1OUT |= LED_RED;
    P1OUT &= ~LED_GREEN;
  }

  if(p2val * S2){
    P1OUT |= LED_GREEN;
    P1OUT &= ~LED_RED;
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
