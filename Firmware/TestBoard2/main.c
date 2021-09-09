#include <msp430.h>


#define bit_notEN 0x04 // P2.2
#define bit_A 0x01 // P2.0
#define bit_B 0x02 // P2.1

#define bit_LED 0x04 // P1.2
#define bit_notSHDN_BOOST 0x20 // P2.5

void forward() {
    // Connect E1 to IOUT-B and E2 to AGND-B => X=X0, Y=Y0
    P2OUT = bit_notEN; // A = 0, B = 0
    P2OUT &= ~bit_notEN;
    P1OUT = 0x01;
}

void reverse() {
    // Connect E1 to AGND-B and E2 to IOUT-B => X=X1 (or X2), Y=Y1
    P2OUT = bit_notEN + bit_A;
    P2OUT &= ~bit_notEN;
    P1OUT = 0x01;
}

void ground() {
    // Connect E1 to AGND-B and E2 to AGND-B => X=X0 Y=Y0
    P2OUT = bit_notEN;
    P2OUT &= ~bit_notEN;
    P1OUT = 0x00;

}

void full_pulse() {
    P1OUT = 0x0;
    P2OUT &= ~bit_notEN; // Enable output
    __delay_cycles(100);
    // Connect E1 to IOUT and E2 to AGND => X=X1, Y=Y1
    P2OUT = bit_notSHDN_BOOST + bit_A; //~bit_notEN, A=1, B= 0
    __delay_cycles(59);
    // Connect E1 to AGND and E2 to IOUT => X=X2 , Y=Y2
    P2OUT = bit_notSHDN_BOOST + bit_B; // ~bit_notEN, A=0, B=1
    __delay_cycles(59);
    // Connect E1 to AGND and E2 to AGND => X=X0 Y=Y0
    P2OUT = bit_notSHDN_BOOST + 0; // ~bit_notEN, A=0, B=0
    __delay_cycles(100);
    P2OUT = bit_notSHDN_BOOST + bit_notEN;
    P1OUT = bit_LED;
}



int main(void)
{
//  BCSCTL1 |= DIVA_1;                        // ACLK/2
//  BCSCTL3 |= LFXT1S_2;                      // ACLK = VLO
  WDTCTL = WDT_ADLY_16;                   // Interval timer
  IE1 |= WDTIE;                             // Enable WDT interrupt
  P2DIR = bit_notEN + bit_A + bit_B + bit_notSHDN_BOOST;  // P2.x outputs
  P2OUT = bit_notEN;                                // All P2.x reset
  P1DIR = bit_LED;
  P1OUT = bit_LED;

  P2OUT = bit_notSHDN_BOOST + bit_notEN; // Turn on pump - TODO: make PWM


  while(1)
  {
//    forward();
//    __delay_cycles(58);
//    ground();
//    __delay_cycles(10);
//    reverse();
//    __delay_cycles(58);
//    ground();
      full_pulse();
    __bis_SR_register(LPM3_bits + GIE);     // Enter LPM3

  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(WDT_VECTOR))) watchdog_timer (void)
#else
#error Compiler not supported!
#endif
{
  __bic_SR_register_on_exit(LPM3_bits);     // Clear LPM3 bits from 0(SR)
}
