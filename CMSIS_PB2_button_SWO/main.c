#include "stm32f10x.h"
#include <stdio.h>
#define LED_PB2_ON() GPIOB->BSRR |= GPIO_BSRR_BS2;
#define LED_PB2_OFF() GPIOB->BSRR |= GPIO_BSRR_BR2;

uint8_t Button_State = 0;

void PINA_0_INIT (void)//Button PA0
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPAEN;//RCC
	
	GPIOA->CRL &= ~GPIO_CRL_MODE0_0;//¬ход (значение после сброса);
	GPIOA->CRL &= ~GPIO_CRL_MODE0_1;//¬ход (значение после сброса);   
  
	GPIOA->CRL &= ~GPIO_CRL_CNF0_0;//10: Input with pull-up / pull-down Ч вход с подт€жкой к питанию или к земле;
  GPIOA->CRL |= GPIO_CRL_CNF0_1; //10: Input with pull-up / pull-down Ч вход с подт€жкой к питанию или к земле;  
}
void PINB_2_INIT (void) //LED PB2
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPBEN;//RCC
	
	GPIOB->CRL &= ~GPIO_CRL_MODE2_0;//0: ¬ыход, максимальна€ частота 2 MHz;
	GPIOB->CRL |=  GPIO_CRL_MODE2_1;//1: ¬ыход, максимальна€ частота 2 MHz;
                          
  GPIOB->CRL &= ~GPIO_CRL_CNF2_0;//00: General purpose output push-pull Ч выход в режиме Push-pull;                          
  GPIOB->CRL &= ~GPIO_CRL_CNF2_1;//00: General purpose output push-pull Ч выход в режиме Push-pull;
}
int main (void)
{
	PINB_2_INIT();
  PINA_0_INIT();
  
//  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_AFIOEN); // set 1
//  CLEAR_BIT(AFIO->MAPR,AFIO_MAPR_SWJ_CFG);   // set 0
//  SET_BIT(AFIO->MAPR, AFIO_MAPR_SWJ_CFG_JTAGDISABLE);
  
 	while(1)
	{
    Button_State = READ_BIT(GPIOA->IDR, GPIO_IDR_IDR0); 
   
    if(Button_State == 1)
      {
       LED_PB2_ON();
			printf("LED_PB2_ON\n");
      } 
      else 
      {
        LED_PB2_OFF();
			printf("LED_PB2_OFF\n");     
      }
	}
}
