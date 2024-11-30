#include "stm32f1xx.h"

#define LED_PB2_ON() GPIOB->BSRR |= GPIO_BSRR_BS2;
#define LED_PB2_OFF() GPIOB->BSRR |= GPIO_BSRR_BR2;

uint8_t Button_State = 0;

void PINA_0_INIT (void)
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPAEN;

	GPIOA->CRL &= ~GPIO_CRL_MODE0_0;
	GPIOA->CRL &= ~GPIO_CRL_MODE0_1;

	GPIOA->CRL &= ~GPIO_CRL_CNF0_0;
    GPIOA->CRL |= GPIO_CRL_CNF0_1;
}
void PINB_2_INIT (void)
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPBEN;

	GPIOB->CRL &= ~GPIO_CRL_MODE2_0;
	GPIOB->CRL |=  GPIO_CRL_MODE2_1;

    GPIOB->CRL &= ~GPIO_CRL_CNF2_0;
    GPIOB->CRL &= ~GPIO_CRL_CNF2_1;
}
int main (void)
{
	PINB_2_INIT();
  PINA_0_INIT();

 	while(1)
	{
    Button_State = READ_BIT(GPIOA->IDR, GPIO_IDR_IDR0);

    if(Button_State == 1)
      {
       LED_PB2_ON();
      }
      else
      {
        LED_PB2_OFF();
      }
	}
}
