#include "stm32f10x.h"
#include "string.h" //для работы функции memcpy

#define LED_PB2_ON() GPIOB->BSRR |= GPIO_BSRR_BS2;
#define LED_PB2_OFF() GPIOB->BSRR |= GPIO_BSRR_BR2;

#define RXSIZE 20
uint8_t ReceivedBuffer[20];
uint8_t MainBuffer[50];
uint8_t Index = 0;

void UART1_Config (void)// USART1 настройка:
{
		// Инициализация портов тактирования.Порт А включили и подключили использование альтернативных функций.Порт B и С включили
		RCC->APB2ENR  |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN| RCC_APB2ENR_IOPCEN; 
																						
		// Настройка вывода на передачу
		GPIOA->CRH		&= ~GPIO_CRH_CNF9;				// Clear CNF bit 9
		GPIOA->CRH		|= GPIO_CRH_CNF9_1;				// Set CNF bit 9 to 10 - AFIO Push-Pull
		GPIOA->CRH		|= GPIO_CRH_MODE9_0;

		// Настройка вывода на прием
		GPIOA->CRH		&= ~GPIO_CRH_CNF10;				// Clear CNF bit 10
		GPIOA->CRH		|= GPIO_CRH_CNF10_0;			// Set CNF bit 10 to 01 HiZ
		GPIOA->CRH		&= ~GPIO_CRH_MODE10;
 												
		// Настройка USART1 регистрами
		RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;	// USART1 Clock ON
		USART1->BRR 	= 0x271;									// Baud rate of 115200, PCLK at 72MHz
		USART1->CR1 	|= USART_CR1_UE |  USART_CR1_RE;	
		// USART1 ON, TX ON, RX ON - включить юсарт, разрешаем передачу и прием данных 

		// настройка вывода PC13 BluePill:
		//GPIOC->CRH &= ~GPIO_CRH_MODE13;   //MODE
		//GPIOC->CRH &= ~GPIO_CRH_CNF13;    //CNF
		//GPIOC->CRH |=  GPIO_CRH_MODE13_1; //2MHz
		//GPIOC->CRH &= ~GPIO_CRH_CNF13; 

		// Настройка вывода PB2 BluePill+
		GPIOB->CRL &= ~GPIO_CRL_MODE2_0;//Выход, максимальная частота 2 MHz;
		GPIOB->CRL |=  GPIO_CRL_MODE2_1;//Выход, максимальная частота 2 MHz;
		GPIOB->CRL &= ~GPIO_CRL_CNF2_0;//General purpose output push-pull — выход в режиме Push-pull;                          
		GPIOB->CRL &= ~GPIO_CRL_CNF2_1;//General purpose output push-pull — выход в режиме Push-pull;	

		// Включение "Разрешение передачи через DMA"
		USART1->CR3 |= (1<<7); 
		// Включение "Разрешение приема через DMA" 
		USART1->CR3 |= (1<<6);  
}

void DMA_Init (void)
{
	
	// Включите тактирование DMA1
	RCC->AHBENR |= 1<<0;
	
	// Включите прерывания DMA
	DMA1_Channel5->CCR |= (1<<1)|(1<<2)|(1<<3);  // TCIE, HTIE, TEIE - биты разрешения прерывания
	
	// Установите направление передачи данных
	DMA1_Channel5->CCR &= ~(1<<4);   // Читаем биты из переферии
	
	// Включите циклический режим (CIRC)
	DMA1_Channel5->CCR |= 1<<5;     //Включение циклического режима для DMA
	
	// Включите увеличение объема памяти (MINC)
	DMA1_Channel5->CCR |= 1<<7;     //Увеличиваем адрес памяти для сохраения пришедших данных
	
	// Установите размер данных периферии (PSIZE)
	DMA1_Channel5->CCR &= ~(3<<8);  // 00 : 8 Bit Data
	
	// Установите размер данных в памяти (MSIZE)
	DMA1_Channel5->CCR &= ~(3<<10);  // 00 : 8 Bit Data - размер данных для памяти
	
	// Установите уровень приоритета
	DMA1_Channel5->CCR &= ~(3<<12);  // PL = 0 - приоритет канала, низкий. Актуально когда больше 1-го используется каналов DMA
}

	//source - адрес источника; destination - адрес назначения; datasize - размер данных
void DMA_Config (uint32_t source, uint32_t destination, uint16_t datasize)
{
	// Установите размер данных в регистре CNTR
	DMA1_Channel5->CNDTR = datasize;
	
	// Установите периферийный адрес в регистре PAR
	DMA1_Channel5->CPAR = source;
	
	// Установите адрес памяти в регистре MAR
	DMA1_Channel5->CMAR = destination;
	
	// Включите DMA 1
	DMA1_Channel5->CCR |= 1<<0;
}

void DMA1_Channel5_IRQHandler (void)
{

	// Если установлено прерывание на половину передачи, то
	if ((DMA1->ISR)&(1<<18))  
	{
		memcpy (&MainBuffer[Index], &ReceivedBuffer[0], RXSIZE/2);
		DMA1->IFCR |= (1<<18); //Очистка бита прерывания заполнения половины буфера
		Index = Index+(RXSIZE/2);
		if (Index>49) 
				{
					Index=0; //Если он превысит размер основного буфера мы сбсроим его
				}
		LED_PB2_ON();
	}


	// Если установлено прерывание завершения полной передачи, то
	if ((DMA1->ISR)&(1<<17))  
		{
		memcpy (&MainBuffer[Index], &ReceivedBuffer[RXSIZE/2], RXSIZE/2);
		DMA1->IFCR |= (1<<17); //Очистка бита прерывания
		Index = Index+(RXSIZE/2);
		if (Index>49) 
				{
					Index=0; //Если он превысит размер основного буфера мы сбсроим его
				}
		LED_PB2_OFF();
		}
}

int main ()
{
	UART1_Config ();
	DMA_Init ();
	
	NVIC_SetPriority (DMA1_Channel5_IRQn, 0); //Установка приорита прерывания
	NVIC_EnableIRQ (DMA1_Channel5_IRQn);      //Включение прерывания по DMA1_15
	
	// Функция пересылки данных из регистра DR в буфер ReceivedBuffer
	DMA_Config ((uint32_t) &USART1->DR, (uint32_t) ReceivedBuffer, RXSIZE);
	
	while (1)
	{

	}
}
