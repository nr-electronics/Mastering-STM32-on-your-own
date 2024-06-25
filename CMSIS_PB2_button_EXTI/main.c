#include "stm32f10x.h"

#define LED_PB2_ON() GPIOB->BSRR |= GPIO_BSRR_BS2;
#define LED_PB2_OFF() GPIOB->BSRR |= GPIO_BSRR_BR2;

uint8_t Button_State = 0;
void Interrupt_EXTI_PA0_init(void);
void PINA_0_INIT (void);//Button PA0
void PINB_2_INIT (void);//LED PB2

void PINA_0_INIT (void)//Button PA0
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPAEN;//RCC
	
	GPIOA->CRL &= ~GPIO_CRL_MODE0_0;//Вход (значение после сброса);
	GPIOA->CRL &= ~GPIO_CRL_MODE0_1;//Вход (значение после сброса);   
  
	GPIOA->CRL &= ~GPIO_CRL_CNF0_0;//10: Input with pull-up / pull-down — вход с подтяжкой к питанию или к земле;
  GPIOA->CRL |= GPIO_CRL_CNF0_1; //10: Input with pull-up / pull-down — вход с подтяжкой к питанию или к земле;                       
  GPIOA->BSRR = GPIO_BSRR_BR9;//подтяжка к земле
}
void PINB_2_INIT (void) //LED PB2
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPBEN;//RCC
	
	GPIOB->CRL &= ~GPIO_CRL_MODE2_0;//0: Выход, максимальная частота 2 MHz;
	GPIOB->CRL |=  GPIO_CRL_MODE2_1;//1: Выход, максимальная частота 2 MHz;
                          
  GPIOB->CRL &= ~GPIO_CRL_CNF2_0;//00: General purpose output push-pull — выход в режиме Push-pull;                          
  GPIOB->CRL &= ~GPIO_CRL_CNF2_1;//00: General purpose output push-pull — выход в режиме Push-pull;
}
void Interrupt_EXTI_PA0_init(void)
{
	EXTI->PR   |= EXTI_PR_PR0;  //Сбрасываем флаг прерывания  перед включением самого прерывания
	EXTI->IMR  |= EXTI_IMR_MR0; //Включаем прерывание 0-го канала EXTI - выставляем маску
	AFIO->EXTICR [0] &= ~AFIO_EXTICR1_EXTI0_PA; //Нулевой канал EXTI подключен к порту PA0	
	RCC->APB2ENR |=RCC_APB2ENR_IOPAEN;//RCC AFIO PortA
	EXTI->FTSR |= EXTI_FTSR_TR0;//Прерывание по спаду импульса
	NVIC_EnableIRQ(EXTI0_IRQn);  //Разрешаем прерывание в контроллере прерываний
	NVIC_SetPriority(EXTI0_IRQn, 0);//Установка приоритета прерывания
}
void EXTI0_IRQHandler(void)
{
		EXTI->PR |= EXTI_PR_PR0; //Сбрасываем флаг прерывания записью "1"
		if(Button_State == 1) // можно проверить по флагу прерывания (EXTI->PR & EXTI_PR_PR0)
    {
        LED_PB2_ON(); 
				for (int i = 0; i < 10000000; i++) { };
    }
}
int main (void)
{
	PINB_2_INIT();
  PINA_0_INIT();
	Interrupt_EXTI_PA0_init();
  
 	while(1)
	{
		Button_State =((GPIOA->IDR) & (GPIO_IDR_IDR0));
	//Button_State = READ_BIT(GPIOA->IDR, GPIO_IDR_IDR0); 
		LED_PB2_OFF()
 	}
}
