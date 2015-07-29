/*
 * pwm_converter.cpp
 *
 * Created: 29.07.2015 14:06:33
 *  Author: ultrablox
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>
extern "C" {
	#include <i2c.h>		// include i2c support
}

/*
Makes from Atmega168 PWM-reader with i2c interface.
*/

const unsigned channel_count = 8;

class pwm_storage
{
	enum class change_t {rise, fall, invalid};
		
	struct channel
	{
		channel()
			:m_val(0.0f)
		{
		}
		
		float value() const
		{
			float res = 0.0f;
			ATOMIC_BLOCK(ATOMIC_FORCEON) {
				res = m_val;
			}
			return res;
		}
		
		void process_change(change_t _change)
		{
			switch(_change)
			{
				case change_t::rise:
					m_riseTime = (TCNT1H << 8) | TCNT1L;
					break;
				case change_t::fall:	//Calculate value
					uint32_t fall_time = (TCNT1H << 8) | TCNT1L;
					uint32_t pwm_width = calc_pwm_width(fall_time, m_riseTime);
					m_val = (float)pwm_width / 1000.0f;
					break;
			}
		}
		
		uint32_t calc_pwm_width(uint32_t rise_time, uint32_t fall_time)
		{
			if(rise_time < fall_time)
				return fall_time - rise_time;
			else
				return (0xffff - rise_time) + fall_time;
		}
	private:
		float m_val;
		uint32_t m_riseTime;
	};
	
	enum class pin_t {B0, B1, B2, B3, B4, B5, B6, B7, C0, C1, C2, C3, C4, C5, C6, C7, D0, D1, D2, D3, D4, D5, D6, D7, Invalid};
	
public:
	pwm_storage()
		:m_portB(PINB), m_portC(PINC), m_portD(PIND)
	{
	}
	
	float value(unsigned channel_index) const
	{
		return m_channels[channel_index].value();
	}	
	
	void process_interruption(uint8_t intr_numb)
	{
		pin_t start_pin = pin_t::Invalid;
		uint8_t changed_bit = 0;
		change_t change = change_t::invalid;
		
		//Get changed bit and channel
		switch(intr_numb)
		{
			case PCINT0_vect_num:	//Port B
			{
				start_pin = pin_t::B0;
				changed_bit = PINB ^ m_portB;
				m_portB = PINB;
				
				change = PINB & changed_bit ? change_t::rise : change_t::fall;
				break;
			}
			case PCINT1_vect_num:
			{
				start_pin = pin_t::C0;
				changed_bit = PINC ^ m_portC;
				m_portC = PINC;
				
				change = PINC & changed_bit ? change_t::rise : change_t::fall;
				break;
			}
			case PCINT2_vect_num:
			{
				start_pin = pin_t::D0;
				changed_bit = PIND ^ m_portD;
				m_portB = PIND;
				
				change = PIND & changed_bit ? change_t::rise : change_t::fall;
				break;
			}
			default:
				break;
		}
		
		pin_t changed_pin = static_cast<pin_t>(static_cast<uint8_t>(start_pin) + positive_bit_index(changed_bit));
	}
private:
	uint8_t positive_bit_index(uint8_t val)
	{
		uint8_t res = 0;
		while(val != 0x01)
		{
			++res;
			val = val >> 1;		
		}
		return res;
	}
private:
	channel m_channels[channel_count];
	uint8_t m_portB, m_portC, m_portD;
};

pwm_storage strg;

void i2c_data_receive(u08 receiveDataLength, u08* receiveData)
{
	/*u08 i;

	// this function will run when a master somewhere else on the bus
	// addresses us and wishes to write data to us

	showByte(*receiveData);
	cbi(PORTB, PB6);

	// copy the received data to a local buffer
	for(i=0; i<receiveDataLength; i++)
	{
		localBuffer[i] = *receiveData++;
	}
	localBufferLength = receiveDataLength;*/
}


u08 i2c_data_transmit(u08 transmitDataLengthMax, u08* transmitData)
{
	/*u08 i;

	// this function will run when a master somewhere else on the bus
	// addresses us and wishes to read data from us

	showByte(*transmitData);
	cbi(PORTB, PB7);

	// copy the local buffer to the transmit buffer
	for(i=0; i<localBufferLength; i++)
	{
		*transmitData++ = localBuffer[i];
	}

	localBuffer[0]++;

	return localBufferLength;*/
	return 0;
}

int main(void)
{
	//Start timer
	TCNT1H  = 0x00;	//Initial value for counter
	TCNT1L  = 0x00;
	
	TCCR1B |= ((1 << CS10) | (0 << CS11) | (0 << CS12));	// start timer1 with /1 prescaler
	
	/**
	 * Pin Change Interrupt enable on PCINT0 (PB0)
	 */
	PCICR = (1 << PCIE0) | //PCINT0..7
				(1 << PCIE1) |	//PCINT8..14
				(1 << PCIE2);	//PCINT16..23
	
	PCMSK0 = (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3);	//PCINT0..7

	// Turn interrupts on.
	sei();
	
	//Setup I2C
	i2cInit();
	i2cSetLocalDeviceAddr(0x91, TRUE);
	i2cSetSlaveReceiveHandler(i2c_data_receive);
	i2cSetSlaveTransmitHandler(i2c_data_transmit);
	
    while(1)
    {
        //TODO:: Please write your application code 
    }
}

ISR(PCINT0_vect)
{ 
	strg.process_interruption(PCINT0_vect_num);
}

ISR(PCINT1_vect)
{
	strg.process_interruption(PCINT1_vect_num);
}

ISR(PCINT2_vect)
{
	strg.process_interruption(PCINT2_vect_num);
}
