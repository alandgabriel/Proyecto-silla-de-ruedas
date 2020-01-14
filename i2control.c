#include <msp430.h>
#define TX		BIT2
#define RX	  BIT1
#define SDA 	BIT7
#define SCL 	BIT6

void __init_i2c(void);
void __init_UART(void);

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;			//deshabilita el timer whatchdog
	__init_UART();
	__init_i2c();
	P1SEL |= RX + TX + SCL + SDA;
	P1SEL2 |= RX + TX + SCL + SDA;			// habilita los pines 1, 2, 6 y 7 del puerto 1 con sus funciones secundarias (RX, TX, scl y sda)
	UCB0CTL1 &= ~UCSWRST;			// libera el reset del USCI
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
	IE2 |= UCB0TXIE + UCA0RXIE;			//habilita interrupcion de transmision del USCI sincrono y de recepcion del usci asincrono
	UCB0I2CSA = 0x28;			//direccion del potenciometro digital max5387
	UCB0CTL1 |= UCTR + UCTXSTT;			//configra USCI en modo transmision y genera el inicio de la transmision si el bus esta disponible
  UCB0TXBUF = 0x010;                        // **************
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
  while (!(IFG2&UCB0TXIFG));                // USCI_A0 TX buffer ready?
}

void __init_i2c(void){
	UCB0CTL1 = UCSWRST;				//habilita el reset por software del modulo USCI
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;			//configura el USCI para i2c y master
	UCB0CTL1 = UCSSEL_2 + UCSWRST;			//reloj SMCLK y mantener el reset del USCI
	UCB0BR0 = 10;					// dividir la frecuencia del reloj 10 veces para tener 100khz
	UCB0BR1 = 0;
}

void __init_UART(void){
	DCOCTL = 0; // Select lowest DCOx and MODx settings<
	BCSCTL1 = CALBC1_1MHZ; // Set DCO
	DCOCTL = CALDCO_1MHZ;
	UCA0CTL1 = UCSWRST;
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 8;                              // 1MHz 115200
  UCA0BR1 = 0;                              // 1MHz 115200
  UCA0MCTL = UCBRS2 + UCBRS0;               // Modulation UCBRSx = 5

}
