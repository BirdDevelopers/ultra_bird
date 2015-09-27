
#include "serial_stream.h"
#include <stm32f10x.h>
#include <stdlib.h>
#include <string.h>

const serial_stream_stm32::endl_t serial_stream::endl;

serial_stream_stm32 * serial_stream_stm32::pSS = 0;

serial_stream_stm32::serial_stream_stm32()
	:curPos(0)
{	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	GPIO_InitTypeDef gpio;
	
	/* Configure USART1 Rx (PA10) as input floating                         */
	gpio.GPIO_Pin   = GPIO_Pin_10;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);

	/* Configure USART1 Tx (PA9) as alternate function push-pull            */
	gpio.GPIO_Pin   = GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);
	
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate            = 57600;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
  
	
	NVIC_InitTypeDef NVIC_InitStructure;                            // Configure the NVIC (nested vector interrupt controller)   
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 	// we want to configure the USART1 interrupts*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	// this sets the priority group of the USART1 interrupts
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 		// this sets the subpriority inside the group
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  		// the USART1 interrupts are globally enabled
  NVIC_Init(&NVIC_InitStructure); 
	
	pSS = this;
}

serial_stream_stm32::~serial_stream_stm32()
{
	USART_Cmd(USART1, DISABLE);
}

void serial_stream_stm32::append_buffer(uint8_t val)
{
	inBuffer[curPos++] = val;
	inBuffer[curPos] = 0;
}

void serial_stream_stm32::clear_buffer()
{
	curPos = 0;
}

void serial_stream_stm32::write(uint8_t c)
{
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void serial_stream_stm32::read(uint16_t & val)
{
	uint8_t buffer[64], cur_val;
	//memset(buffer, 0x00, sizeof(buffer));
	
	unsigned cur_pos = 0;
	do
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
		cur_val = USART_ReceiveData(USART1);
		
		if(cur_val == 0x0D)
		{
			buffer[cur_pos] = 0x00;
			write(cur_val);
		}
		else if(cur_pos < sizeof(buffer))
		{
			buffer[cur_pos++] = cur_val;
			write(cur_val);
		}
	} while(cur_val != 0x0D); //Not 'return'
	
	val = atoi((const char*)buffer);
}

void serial_stream_stm32::write(int val)
{
	write_basetype(val);
}

void serial_stream_stm32::write(unsigned val)
{
	write_basetype(val);
}

void serial_stream_stm32::write(const char * p_str)
{
	while(*p_str)
	{
		write((uint8_t)*p_str++);
	}
}

void serial_stream_stm32::write(const serial_stream::endl_t &)
{
	write((uint8_t)'\n');
	write((uint8_t)'\r');
}

extern "C" {
	
void USART1_IRQHandler (void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {	
		uint8_t val = USART_ReceiveData(USART1);
		serial_stream_stm32::pSS->append_buffer(val);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
		if(val != 0x0D)
			serial_stream_stm32::pSS->write(val);
		else
			*serial_stream_stm32::pSS << serial_stream::endl;
  }
}
};

/*
void serial_stream_stm32::write(const std::string & str)
{
	//for(unsigned i = 0; i < str.size(); ++i)
	//	write((uint8_t)str[i]);
}*/
