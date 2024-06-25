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
	
	GPIOA->CRL &= ~GPIO_CRL_MODE0_0;//���� (�������� ����� ������);
	GPIOA->CRL &= ~GPIO_CRL_MODE0_1;//���� (�������� ����� ������);   
  
	GPIOA->CRL &= ~GPIO_CRL_CNF0_0;//10: Input with pull-up / pull-down � ���� � ��������� � ������� ��� � �����;
  GPIOA->CRL |= GPIO_CRL_CNF0_1; //10: Input with pull-up / pull-down � ���� � ��������� � ������� ��� � �����;                       
  GPIOA->BSRR = GPIO_BSRR_BR9;//�������� � �����
}
void PINB_2_INIT (void) //LED PB2
{
	RCC->APB2ENR |=RCC_APB2ENR_IOPBEN;//RCC
	
	GPIOB->CRL &= ~GPIO_CRL_MODE2_0;//0: �����, ������������ ������� 2 MHz;
	GPIOB->CRL |=  GPIO_CRL_MODE2_1;//1: �����, ������������ ������� 2 MHz;
                          
  GPIOB->CRL &= ~GPIO_CRL_CNF2_0;//00: General purpose output push-pull � ����� � ������ Push-pull;                          
  GPIOB->CRL &= ~GPIO_CRL_CNF2_1;//00: General purpose output push-pull � ����� � ������ Push-pull;
}
void Interrupt_EXTI_PA0_init(void)
{
	EXTI->PR   |= EXTI_PR_PR0;  //���������� ���� ����������  ����� ���������� ������ ����������
	EXTI->IMR  |= EXTI_IMR_MR0; //�������� ���������� 0-�� ������ EXTI - ���������� �����
	AFIO->EXTICR [0] &= ~AFIO_EXTICR1_EXTI0_PA; //������� ����� EXTI ��������� � ����� PA0	
	RCC->APB2ENR |=RCC_APB2ENR_IOPAEN;//RCC AFIO PortA
	EXTI->FTSR |= EXTI_FTSR_TR0;//���������� �� ����� ��������
	NVIC_EnableIRQ(EXTI0_IRQn);  //��������� ���������� � ����������� ����������
	NVIC_SetPriority(EXTI0_IRQn, 0);//��������� ���������� ����������
}
void EXTI0_IRQHandler(void)
{
		EXTI->PR |= EXTI_PR_PR0; //���������� ���� ���������� ������� "1"
		if(Button_State == 1) // ����� ��������� �� ����� ���������� (EXTI->PR & EXTI_PR_PR0)
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
