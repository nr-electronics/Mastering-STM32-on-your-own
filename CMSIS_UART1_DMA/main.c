#include "stm32f10x.h"
#include "string.h" //��� ������ ������� memcpy

#define LED_PB2_ON() GPIOB->BSRR |= GPIO_BSRR_BS2;
#define LED_PB2_OFF() GPIOB->BSRR |= GPIO_BSRR_BR2;

#define RXSIZE 20
uint8_t ReceivedBuffer[20];
uint8_t MainBuffer[50];
uint8_t Index = 0;

void UART1_Config (void)// USART1 ���������:
{
		// ������������� ������ ������������.���� � �������� � ���������� ������������� �������������� �������.���� B � � ��������
		RCC->APB2ENR  |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN| RCC_APB2ENR_IOPCEN; 
																						
		// ��������� ������ �� ��������
		GPIOA->CRH		&= ~GPIO_CRH_CNF9;				// Clear CNF bit 9
		GPIOA->CRH		|= GPIO_CRH_CNF9_1;				// Set CNF bit 9 to 10 - AFIO Push-Pull
		GPIOA->CRH		|= GPIO_CRH_MODE9_0;

		// ��������� ������ �� �����
		GPIOA->CRH		&= ~GPIO_CRH_CNF10;				// Clear CNF bit 10
		GPIOA->CRH		|= GPIO_CRH_CNF10_0;			// Set CNF bit 10 to 01 HiZ
		GPIOA->CRH		&= ~GPIO_CRH_MODE10;
 												
		// ��������� USART1 ����������
		RCC->APB2ENR	|= RCC_APB2ENR_USART1EN;	// USART1 Clock ON
		USART1->BRR 	= 0x271;									// Baud rate of 115200, PCLK at 72MHz
		USART1->CR1 	|= USART_CR1_UE |  USART_CR1_RE;	
		// USART1 ON, TX ON, RX ON - �������� �����, ��������� �������� � ����� ������ 

		// ��������� ������ PC13 BluePill:
		//GPIOC->CRH &= ~GPIO_CRH_MODE13;   //MODE
		//GPIOC->CRH &= ~GPIO_CRH_CNF13;    //CNF
		//GPIOC->CRH |=  GPIO_CRH_MODE13_1; //2MHz
		//GPIOC->CRH &= ~GPIO_CRH_CNF13; 

		// ��������� ������ PB2 BluePill+
		GPIOB->CRL &= ~GPIO_CRL_MODE2_0;//�����, ������������ ������� 2 MHz;
		GPIOB->CRL |=  GPIO_CRL_MODE2_1;//�����, ������������ ������� 2 MHz;
		GPIOB->CRL &= ~GPIO_CRL_CNF2_0;//General purpose output push-pull � ����� � ������ Push-pull;                          
		GPIOB->CRL &= ~GPIO_CRL_CNF2_1;//General purpose output push-pull � ����� � ������ Push-pull;	

		// ��������� "���������� �������� ����� DMA"
		USART1->CR3 |= (1<<7); 
		// ��������� "���������� ������ ����� DMA" 
		USART1->CR3 |= (1<<6);  
}

void DMA_Init (void)
{
	
	// �������� ������������ DMA1
	RCC->AHBENR |= 1<<0;
	
	// �������� ���������� DMA
	DMA1_Channel5->CCR |= (1<<1)|(1<<2)|(1<<3);  // TCIE, HTIE, TEIE - ���� ���������� ����������
	
	// ���������� ����������� �������� ������
	DMA1_Channel5->CCR &= ~(1<<4);   // ������ ���� �� ���������
	
	// �������� ����������� ����� (CIRC)
	DMA1_Channel5->CCR |= 1<<5;     //��������� ������������ ������ ��� DMA
	
	// �������� ���������� ������ ������ (MINC)
	DMA1_Channel5->CCR |= 1<<7;     //����������� ����� ������ ��� ��������� ��������� ������
	
	// ���������� ������ ������ ��������� (PSIZE)
	DMA1_Channel5->CCR &= ~(3<<8);  // 00 : 8 Bit Data
	
	// ���������� ������ ������ � ������ (MSIZE)
	DMA1_Channel5->CCR &= ~(3<<10);  // 00 : 8 Bit Data - ������ ������ ��� ������
	
	// ���������� ������� ����������
	DMA1_Channel5->CCR &= ~(3<<12);  // PL = 0 - ��������� ������, ������. ��������� ����� ������ 1-�� ������������ ������� DMA
}

	//source - ����� ���������; destination - ����� ����������; datasize - ������ ������
void DMA_Config (uint32_t source, uint32_t destination, uint16_t datasize)
{
	// ���������� ������ ������ � �������� CNTR
	DMA1_Channel5->CNDTR = datasize;
	
	// ���������� ������������ ����� � �������� PAR
	DMA1_Channel5->CPAR = source;
	
	// ���������� ����� ������ � �������� MAR
	DMA1_Channel5->CMAR = destination;
	
	// �������� DMA 1
	DMA1_Channel5->CCR |= 1<<0;
}

void DMA1_Channel5_IRQHandler (void)
{

	// ���� ����������� ���������� �� �������� ��������, ��
	if ((DMA1->ISR)&(1<<18))  
	{
		memcpy (&MainBuffer[Index], &ReceivedBuffer[0], RXSIZE/2);
		DMA1->IFCR |= (1<<18); //������� ���� ���������� ���������� �������� ������
		Index = Index+(RXSIZE/2);
		if (Index>49) 
				{
					Index=0; //���� �� �������� ������ ��������� ������ �� ������� ���
				}
		LED_PB2_ON();
	}


	// ���� ����������� ���������� ���������� ������ ��������, ��
	if ((DMA1->ISR)&(1<<17))  
		{
		memcpy (&MainBuffer[Index], &ReceivedBuffer[RXSIZE/2], RXSIZE/2);
		DMA1->IFCR |= (1<<17); //������� ���� ����������
		Index = Index+(RXSIZE/2);
		if (Index>49) 
				{
					Index=0; //���� �� �������� ������ ��������� ������ �� ������� ���
				}
		LED_PB2_OFF();
		}
}

int main ()
{
	UART1_Config ();
	DMA_Init ();
	
	NVIC_SetPriority (DMA1_Channel5_IRQn, 0); //��������� �������� ����������
	NVIC_EnableIRQ (DMA1_Channel5_IRQn);      //��������� ���������� �� DMA1_15
	
	// ������� ��������� ������ �� �������� DR � ����� ReceivedBuffer
	DMA_Config ((uint32_t) &USART1->DR, (uint32_t) ReceivedBuffer, RXSIZE);
	
	while (1)
	{

	}
}
