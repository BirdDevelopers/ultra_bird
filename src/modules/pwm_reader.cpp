
#include "pwm_reader.h"
#include <avr/io.h>
#include <avr/interrupt.h>

pwm_reader * pwm_reader::p_sharedPwmReader = nullptr;

pwm_reader::pwm_reader()
{
	p_sharedPwmReader = this;
	
	for(unsigned i = 0; i < 6; ++i)
		m_data[i] = 0.0f;
}

float pwm_reader::channel_value(unsigned ch_idx) const
{
	return m_data[ch_idx];
}

error_t pwm_reader::init()
{
	 // Start with channel 0
	 ADMUX &= (1<<REFS1) | (1<<REFS0) | (1<<ADLAR);
	 // ADC off
	 ADCSRA &= ~(1 << ADEN);
	 // Bandgap reference and input capture controlled by AC
	 ACSR = (1<<ACBG) | (1<<ACIC);
	 // Use AD mux as the comparator input
	 SFIOR |= (1<<ACME);
	 // Start timer1 with /8 prescaler and select falling edge for
	 // input capture.
	 // Note that the edge sense is reversed because the
	 // AC asserts when the input is lower than AIN0 so this is
	 // to capture the first rising edge for the servo at channel 0.
	 TCCR1B = (1 << CS11);
	 // Enable the input capture interrupt
	 TIMSK |= (1 << TICIE1);
	 
	return error_t::ok();
}

ISR(TIMER1_CAPT_vect)
{
	/*static uint16_t start;
	uint8_t channel = ADMUX & 7;
	uint16_t t = ICR1;
	if ((TCCR1B & (1<<ICES1)) == 0) {
		// Falling edge on signal (i.e., rising on AC) from now
		TCCR1B |= (1<<ICES1);
		} else {
		channelTime[channel] = t - start;
		if (++channel == N_CHANNELS) {
			channel = 0;
			// Back to rising on signal (i.e., falling on AC) for channel 0 start
			TCCR1B &= ~(1<<ICES1);
			nFrames++;
		}
		ADMUX = (ADMUX & ((1<<REFS1) | (1<<REFS0) | (1<<ADLAR))) | channel;
	}
	start = t;*/
}
