#include "stm32f10x.h"

void PORTC_13_INIT (void)
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPCEN;

	GPIOC->CRH &= ~GPIO_CRH_MODE13;   //очистить разряды MODE
	GPIOC->CRH |=  GPIO_CRH_MODE13_1; //выход, 2MHz
	GPIOC->CRH &= ~GPIO_CRH_CNF13;    //очистить разряды CNF и сделать выход в режиме Push-pull;
}

int main (void)
{
	PORTC_13_INIT ();
	
	while(1)
	{
		GPIOC->BSRR |= GPIO_BSRR_BS13;

		for (int i = 0; i < 1000000; i++) { 
		};

		GPIOC->BSRR |= GPIO_BSRR_BR13; 

		for (int i = 0; i < 1000000; i++) {
	  };
  }
	
}

