#include "stm32f0xx_conf.h"
#include "uart.h"
#include "dac_generator.h"

//---------------------------------------------------------------------------------


const uint8_t sin_tab [256] ={128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 
167, 170, 173, 176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 
206, 208, 211, 213, 215, 218, 220, 222, 224, 226, 228, 230, 232, 234, 
235, 237, 238, 240, 241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 
252, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 
254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 
241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 
215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 
179, 176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 
137, 134, 131, 128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 
93, 90, 88, 85, 82, 79, 76, 73, 70, 67, 65, 62, 59, 57, 54, 52, 49, 
47, 44, 42, 40, 37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 
14, 12, 11, 10, 9, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 
0, 1, 1, 1, 2, 2, 3, 4, 5, 5, 6, 7, 9, 10, 11, 12, 14, 15, 17, 18, 
20, 21, 23, 25, 27, 29, 31, 33, 35, 37, 40, 42, 44, 47, 49, 52, 54, 
57, 59, 62, 65, 67, 70, 73, 76, 79, 82, 85, 88, 90, 93, 97, 100, 103, 
106, 109, 112, 115, 118, 121, 124};



//---------------------------------------------------------------------------------

volatile uint32_t dds_frq = 4294967;
volatile uint8_t lock_spi = 0;



uint16_t t_i, t_i2, dta_start, command, act_frq;

//---------------------------------------------------------------------------------

void SysTick_Handler(void) {
    static uint32_t dds_acc = 0;
    uint8_t quadr;

    //************* select lock-in *********************
    quadr = (dds_acc >> 30);
    quadr = quadr ^ (quadr >> 1);

    GPIOB->ODR = ( (GPIOB ->ODR ) &  (~(3 << 4)) ) | ( quadr << 4);

    if (!lock_spi) {
    	Spi_Send_Ampl((sin_tab[dds_acc]));
    } 
    
    dds_acc += dds_frq;
    
}


//---------------------------------------------------------------------------------


void set_freq ( uint16_t freq) { // set output frequency - unit = 0.01Hz
    uint64_t t_f; 
    
    t_f = freq;
    t_f = 0x00000000FFFFFFFF *  t_f;
    t_f = t_f / 20000000UL;
    
    dds_frq = t_f; 
}

//---------------------------------------------------------------------------------

#define COMMAND_FREQ 1
#define COMMAND_FREQ_ASK 2
#define COMMAND_AMP 3
#define COMMAND_AMP_ASK 4


void main(void)
{
	uint16_t act_ampl=0;

	SystemInit();
	SysTick_Config(SystemCoreClock/200000); //przerwanie co 5us

	UARTInite(38400);
	
	
	//NVIC_EnableIRQ(USART1_IRQn);
	
	// ********** Priorytety przerwañ *******************
	NVIC_SetPriority (SysTick_IRQn,2);
	NVIC_SetPriority (USART1_IRQn,0);


	act_frq = 33300;

	set_ampl (0);
	set_freq (33300);
		
	while(1) {
	
	    if (rx_frame) {
		
		dta_start = 0;
		command = 0;
		if ((rx_buf [0] == 'F') && (rx_buf[4] == ' ')) {command = COMMAND_FREQ; dta_start = 5;}
		if ((rx_buf [0] == 'F') && (rx_buf[4] == '?')) {command = COMMAND_FREQ_ASK; dta_start = rx_buf_cnt;}
		if ((rx_buf [0] == 'A') && (rx_buf[3] == ' ')) {command = COMMAND_AMP; dta_start = 4;}
		if ((rx_buf [0] == 'A') && (rx_buf[3] == '?')) {command = COMMAND_AMP_ASK; dta_start = rx_buf_cnt;}

		t_i = 0; 
		for ( t_i2 = dta_start; t_i2 < rx_buf_cnt ; t_i2++ ) 
		    if ( ( rx_buf [t_i2] >= '0' ) && ( rx_buf [t_i2] <= '9' ) ) t_i = t_i * 10 + (rx_buf [t_i2] - '0');


		
		switch (command) {

		    case COMMAND_FREQ : {
			if ( ( t_i >= 500 ) && (t_i <= 60000) )  {
				set_freq ( (uint16_t) t_i);
				act_frq = t_i;
				USART_Send_String("FREQ\r\n");
		    	}
			else {
		    		USART_Send_String("parameter_error\r\n");
		    	}
		    }
		    break;
		    
		    case COMMAND_FREQ_ASK : {
		    USART_Send_Int(act_frq);
			USART_Send_String("\r\n");
		    }
		    break;
		    
		    case COMMAND_AMP : {
			if ( ( t_i >= 0 ) && (t_i <= 5000) )  {
				set_ampl ( (uint8_t) ( (t_i * 255) / 6120 ) );
				act_ampl = t_i;
				USART_Send_String("AMP\r\n");
		    	}
			else {
		    		USART_Send_String ("parameter_error\r\n");
		    	}
		    }
		    break;
    
		    case COMMAND_AMP_ASK : {
		    USART_Send_Int (act_ampl);
		    USART_Send_String("\r\n");
		    }
		    break;
		    
		    default: USART_Send_String("Unknown Command...\r\n");
		    break;

		}
		
		
		rx_buf_cnt = 0;
		rx_frame = 0;

	    } 


	}


}
