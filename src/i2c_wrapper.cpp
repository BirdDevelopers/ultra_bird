
#include "i2c_wrapper.h"
#include <stm32f10x.h>

i2c_wrapper::i2c_wrapper(uint8_t addr)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);
	GPIO_PinRemapConfig( GPIO_Remap_I2C1 , ENABLE );

	/* I2C1 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* I2C1 SDA and SCL configuration */
	GPIO_InitTypeDef  gpio;
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &gpio);
	/*SCL is pin06 and SDA is pin 07 for I2C1*/

	 /* I2C1 configuration */
	I2C_InitTypeDef  I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = addr;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
    
}

i2c_wrapper::~i2c_wrapper()
{
	I2C_DeInit(I2C1);
}

void i2c_wrapper::start_transmission(direction_t dir, uint8_t slave_addr)
{
	uint8_t transmissionDirection = (dir == i2c_wrapper::direction_t::Out ? I2C_Direction_Transmitter : I2C_Direction_Receiver);
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C1, ENABLE);
	I2C_Send7bitAddress(I2C1, slave_addr, transmissionDirection);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
 
	if(transmissionDirection== I2C_Direction_Transmitter)
	{
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}

	if(transmissionDirection == I2C_Direction_Receiver)
	{
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

void i2c_wrapper::stop_transmission()
{
	I2C_GenerateSTOP(I2C1, ENABLE);
}

void i2c_wrapper::write(uint8_t val)
{
	 I2C_SendData(I2C1, val);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}
