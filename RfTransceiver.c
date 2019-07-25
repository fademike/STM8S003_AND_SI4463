/*
 * RfTransceiver.c
 *
 *  Created on: 16 июля 2016 г.
 *      Author: NASA
 */

#include "stm8s.h"

#include "RfTransceiver.h"
#include "radio_config_Si4463.h"
#include "Si4463.h"



//uint8_t * RFbuffer[256];
unsigned char RFbuffer[256];








int8_t getLevel(void)
{
	uint8_t level = 0;

	cmdReadFRR(0, &level);
	//return -((int8_t)(127 - level / 2));
	return level;
}

#define REGISTRS 1

unsigned char getStatus(void)
{
#if REGISTRS
	uint8_t status;
	cmdReadFRR(1, (uint8_t *)&status);
	return status;
#else
	unsigned char buffer[9];
	SI446X_INT_STATUS(buffer);


	return buffer[3];
#endif
}



uint8_t changeState(uint8_t state)
{
	switch (state) {
		case 1:
			SI446X_START_RX(0,0,0,0,3,3);//cmdStartRx(0, 0);
			break;
		case 2:
			SI446X_START_TX(0,0);
			break;
		default:
                        SI446X_START_RX(0,0,0,0,3,3);
			break;
	}

	return 0;
}



uint32_t RFread(uint8_t *data, uint32_t length)
{
	uint8_t bufferSize = 64;
	//SI446X_FIFOINFO(&bufferSize, NULL, 0, 0);
	SI446X_READ_PACKET(data , bufferSize);
	//HAL_UART_Transmit_IT(&huart1, data, bufferSize);
	//HAL_UART_Transmit_DMA(&huart1, data, bufferSize);
	return bufferSize;

}



uint32_t RFwrite(unsigned char *data, uint8_t length)
{

	//SI446X_WRITE_TX_FIFO(data, length);//packetLength);
	//return length;
        
        SI446X_WRITE_TX_FIFO(data, length);

        changeState(STATE_TX);
        //GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
        while((getStatus()&0x20) == 0x00){};
        //GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
        
        changeState(STATE_RX);
}





unsigned char config_table[] = RADIO_CONFIGURATION_DATA_ARRAY;


void MYinit(void)
{

    unsigned int i;
    unsigned int j = 0;
    while( ( i = config_table[j] ) != 0 )
    {
        j++;
        SI446X_CMD( &config_table[j], i );
        j += i;
    }

}




