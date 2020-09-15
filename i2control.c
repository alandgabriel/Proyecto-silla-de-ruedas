#include <msp430.h>
#include <stdbool.h>

#define CS		BIT3
#define INC		BIT4
#define UD		BIT5
bool UP = true;
bool DOWN = false;
unsigned int Nsteps = 100;

void __init_wheelchair(void);
void __init_UART(void);
void set_pot(unsigned int pos, bool save);
void step_pot(unsigned int steps, bool dir);
void desel_save(void);
void desel_notsave(void);

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;			//deshabilita el timer whatchdog

	__init_wheelchair();
	set_pot(75,true);
	__bis_SR_register(CPUOFF + GIE);          // apaga el cpu y habilita las interrupciones generales
	while (1)
	{

	}

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



void __init_wheelchair(void){
	P1DIR = BIT0 + CS + INC + UD;     //Configurar pines del puerto 1 como salida
	P2DIR = BIT0 + BIT1;		//Configurar pines del puerto 2 como salida para ajustar en neutral el control (VS/2)
	P1OUT = 0;
	P2OUT = 0;
	P1OUT |= INC;			      // Activa pin inc del pot digital
	P2OUT |= BIT0 + BIT1;   // Activa las salidas del cto de control de silla a vs/2 a traves de la compuerta analogica
	__delay_cycles(1000000);
	P2OUT &= ~BIT0;
	P2OUT &= ~BIT1;

}

void set_pot(unsigned int pos, bool save)
{
	step_pot (Nsteps, DOWN); 			// Ajusta el wipper al minimo de resistencia dando 100 pasos hacia abajo
	step_pot (pos, UP);
	if (save){
		desel_save();
	}
	else {
		desel_notsave();
	}

}

void step_pot(unsigned int steps, bool dir)
{ P1OUT &= ~CS;
	if (dir==UP)
	{
		P1OUT |= UD;
	}
	else
	{
		P1OUT &= ~UD;
	}
	unsigned int i;
	for (i=0; i<steps; i++)
	{
		P1OUT &= ~INC;
		__delay_cycles(1);
		P1OUT |= INC;
		__delay_cycles(1);
	}
	__delay_cycles(100);
}

void desel_save(void)  				// desactiva el chip select guardando el wipper
{
	P1OUT |= CS;

}

void desel_notsave(void)
{
	P1OUT &= ~INC;
	P1OUT |= CS;
	P1OUT |= INC;

}
