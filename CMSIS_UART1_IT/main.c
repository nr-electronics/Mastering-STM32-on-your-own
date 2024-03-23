#include "stm32f10x.h"

uint32_t temp = 0, i;
char data = 0;

int main(void)
{
		// инициализация всех портов тактирования
		RCC->APB2ENR  |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN| RCC_APB2ENR_IOPCEN;// Порт А включили и подключили использование альтернативных функций
																																																		 // Порт B и С включили

		// USART1 настройка:	
			// настройка вывода на передачу
		GPIOA->CRH		&= ~GPIO_CRH_CNF9;				// Clear CNF bit 9
		GPIOA->CRH		|= GPIO_CRH_CNF9_1;				// Set CNF bit 9 to 10 - AFIO Push-Pull
		GPIOA->CRH		|= GPIO_CRH_MODE9_0;

			// настройка вывода на прием
		GPIOA->CRH		&= ~GPIO_CRH_CNF10;				// Clear CNF bit 9
		GPIOA->CRH		|= GPIO_CRH_CNF10_0;			// Set CNF bit 9 to 01 HiZ
		GPIOA->CRH		&= ~GPIO_CRH_MODE10;
			
			// Настройка USART1 регистрами
		RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;	// USART1 Clock ON
		USART1->BRR 	= 0x1D4C;									// Baudrate for 9600 and 72Mhz-RCC
		USART1->CR1 	|= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE ;	
			// USART1 ON, TX ON, RX ON, RXNEIE - разрешаем прерывание по приему данных

			// настройка вывода PC13 BluePill
		//GPIOC->CRH &= ~GPIO_CRH_MODE13;   //MODE
		//GPIOC->CRH &= ~GPIO_CRH_CNF13;    //CNF
		//GPIOC->CRH |=  GPIO_CRH_MODE13_1; //2MHz
		//GPIOC->CRH &= ~GPIO_CRH_CNF13; 

			// настройка вывода PB2 BluePill+
		GPIOB->CRL &= ~GPIO_CRL_MODE2_0;//Выход, максимальная частота 2 MHz;
		GPIOB->CRL |=  GPIO_CRL_MODE2_1;//Выход, максимальная частота 2 MHz;
															
		GPIOB->CRL &= ~GPIO_CRL_CNF2_0;//General purpose output push-pull — выход в режиме Push-pull;                          
		GPIOB->CRL &= ~GPIO_CRL_CNF2_1;//General purpose output push-pull — выход в режиме Push-pull;

		//инциализация прерывания для USART1
		NVIC_EnableIRQ (USART1_IRQn);
		__enable_irq ();


 while(1)
	{
		for (i=0; i<1000000; ++i) {};  // делаем задержку
		USART1->DR = data++;
		while((USART1->SR & USART_SR_TC)==0) {} // Ждать окончания передачи
    USART1->SR = ~USART_SR_TC; // Очистить флаг окончания передачи
  }
}

void USART1_IRQHandler(void)
{
	if((USART1->SR & USART_SR_RXNE)!=0) // Если приём завершен
	{
		temp = USART1->DR; // Считываем принятый байт
		
		if (temp == '1'){
//			GPIOC->ODR ^= GPIO_ODR_ODR13; //BluePill
				GPIOB->ODR ^= GPIO_ODR_ODR2;  //BluePill+
		}
		else if (temp == '2'){
			USART1->DR = 0xAE; //символ товарный знак
		}
	}
		
}
