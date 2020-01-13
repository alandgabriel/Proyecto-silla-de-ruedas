#include <msp430.h>
#define TXUART BIT2
#define RXUART BIT1
#define TXI2C BIT7
#define RXI2C BIT6
void __init_i2c(void);
void __init_UART(void);

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;			//deshabilita el timer whatchdog
	__init_UART();
	__init_i2c();
	P1SEL |= TXU + BIT7;
	P1SEL2 |= BIT6 + BIT7;			// habilita los pines 6 y 7 del puerto 1 con sus funciones secundarias (scl y sda)
	UCB0CTL1 &= ~UCSWRST;			// libera el reset del USCI
	IE2 |= UCB0TXIE;			//habilita interrupcion de transmision del USCI
	UCB0I2CSA = 0x28;			//direccion del potenciometro digital max5387
	UCB0CTL1 |= UCTR + UCTXSTT;			//configra USCI en modo transmision y genera el inicio de la transmision si el bus esta disponible
	__bis_SR_register(CPUOFF + GIE);          // apaga el cpu y habilita las interrupciones generales
}

/*
// USCI_B0 Data ISR
#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
{

}
*/

// Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
{
  while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = UCA0RXBUF;                    // TX -> RXed character
}

void __init_i2c(void){
	UCB0CTL1 = UCSWRST;				//habilita el reset por software del modulo USCI
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;			//configura el USCI para i2c y master
	UCB0CTL1 = UCSSEL_2 + UCSWRST;			//reloj SMCLK y mantener el reset del USCI
	UCB0BR0 = 10;					// dividir la frecuencia del reloj 10 veces para tener 100khz
	UCB0BR1 = 0;
}

void __init_UART(void){

}
