#include <msp430.h>
#include <stdbool.h>
#define TX		BIT2
#define RX	  BIT1

void __init_UART(void);

unsigned char RXDATA = 0;
unsigned int xPos = 0;
unsigned int yPos = 0;
bool PAR = true;


int main(void) {
	WDTCTL = WDTPW + WDTHOLD;			//deshabilita el timer whatchdog
	__init_UART();
	P1SEL |= RX + TX;
	P1SEL2 |= RX + TX;			// habilita los pines 1, 2, 6 y 7 del puerto 1 con sus funciones secundarias (RX, TX, scl y sda)
	P1DIR |= BIT0 + BIT6;					// P1.6 set as output
	P1OUT &= ~(BIT0 + BIT6);				// P1.6 set low
	UCA0CTL1 &= ~UCSWRST;                     // Liberar el USCI para entrar en operacion
	IE2 |= UCA0RXIE;			//habilita interrupcion de transmision del USCI sincrono y de recepcion del usci asincrono
  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
	while (1)
	{
		if (PAR)
		{
			yPos = RXDATA;
			yPos = (yPos-47)*10;
			if (xPos == 10 && yPos ==100 )
			{
				P1OUT ^= BIT0 + BIT6;			// P1.0 and P1.6 set high

			}


		}

		else
		{
			xPos = RXDATA;
			xPos = (xPos-47)*10;


		}


		__bis_SR_register(LPM0_bits);	// Enter LPM0, interrupts enabled

		}

	}



//  Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  RXDATA = UCA0RXBUF;
	PAR = !PAR;
	__bic_SR_register_on_exit(LPM0_bits);	// Wake-up CPU
}


void __init_UART(void){
	if (CALBC1_1MHZ==0xFF)					// If calibration constant erased
	{
		while(1);                               // do not load, trap CPU!!
	}
	DCOCTL = 0; // Select lowest DCOx and MODx settings<
	BCSCTL1 = CALBC1_1MHZ; // Set DCO
	DCOCTL = CALDCO_1MHZ;
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 104;                              // 1MHz/ 9615
  UCA0BR1 = 0;                              // 1MHz /9615
  UCA0MCTL = UCBRS0;               // Modulation UCBRSx = 1
}


/*switch (RXDATA)
{
	case 49:							// ON Command
		P1OUT |= BIT0 + BIT6;			// P1.0 and P1.6 set high

		break;

	case 53:							// OFF Command

		P1OUT &= ~(BIT0 + BIT6);		// P1.0 and P1.6 set low
		break;

	default: break;
}*/
