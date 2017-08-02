/*
 * Si4463.c
 *
 *  Created on: 16 июля 2016 г.
 *      Author: NASA
 */

//#include "stm32f4xx_hal.h"
#include "stm8s.h"
#include "si446x_defs.h"
#include "Si4463.h"

//extern SPI_HandleTypeDef hspi2;


void SPI_SendData(uint8_t Data);
uint8_t SPI_ReceiveData(void);

unsigned char SPI_TxRxData(unsigned char request){
  unsigned char answer=0;
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    SPI_SendData(request);//SPI_SendData(cTemp);//while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET){};//HAL_SPI_Transmit(&hspi2, &cTemp, 1, 100);
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){}; answer = SPI_ReceiveData();
    //while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    
    return answer;
}


#define READ_CMD_BUFF 0x44

void SI446X_WAIT_CTS( void )
{
	unsigned char cTemp = 0x44;
	unsigned char cts;

    do
    {
    	GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_RESET);	//SI_CSN_LOW( );
        /*while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
        SPI_SendData(cTemp);//HAL_SPI_Transmit(&hspi2, &cTemp, 1, 100);
        //while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET){};
        while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){}; SPI_ReceiveData();
        while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
        SPI_SendData(0x00); while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){};
        cts = SPI_ReceiveData();//HAL_SPI_Receive(&hspi2, &cts, 1, 100);
        */
        SPI_TxRxData(cTemp);
        cts = SPI_TxRxData(0x00);
        
        GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_SET); //SI_CSN_HIGH( );

        //if(GPIOA->IDR & SI_PIO_1_Pin) HAL_GPIO_WritePin(GPIOA, PTT_Pin, GPIO_PIN_RESET);
        //else HAL_GPIO_WritePin(GPIOA, PTT_Pin, GPIO_PIN_SET);
    } while( cts != 0xFF );
    
    
}




void SI446X_CMD(const unsigned char *cmd, unsigned char cmdsize )
{
    unsigned char i=0;
    SI446X_WAIT_CTS( );
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_RESET);	//SI_CSN_LOW( );
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    while(i<cmdsize){SPI_SendData(cmd[i]); while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){}; SPI_ReceiveData(); i++; while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};}//HAL_SPI_Transmit(&hspi2, cmd, cmdsize, 100);

    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_SET); //SI_CSN_HIGH( );
}


void SI446X_READ_RESPONSE( unsigned char *buffer, unsigned char size )
{
	unsigned char cTemp = 0x44;
    unsigned char i=0;
	//unsigned char cts = 0x00;
    for (i=0;i<size;i++)buffer[i] = 0x00;
    i=0;
    
    SI446X_WAIT_CTS( );
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_RESET);	//SI_CSN_LOW( );
    
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    SPI_SendData(cTemp);//while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET){};//HAL_SPI_Transmit(&hspi2, &cTemp, 1, 100);
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){}; SPI_ReceiveData();
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    
    while(i<size){SPI_SendData(0x00);while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){};
      
      buffer[i++] = SPI_ReceiveData();}//HAL_SPI_Receive(&hspi2, buffer, size, 100);
    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_SET); //SI_CSN_HIGH( );
}


void SI446X_TX_FIFO_RESET( void )
{
	unsigned char cmd[2+1];
    cmd[0] = FIFO_INFO;		//FIFO_INFO = 0x15
    cmd[1] = 0x01;
    SI446X_CMD( cmd, 2 );
    SI446X_READ_RESPONSE(cmd, 2);
}


void SI446X_RX_FIFO_RESET( void )
{
	unsigned char cmd[2+1];
    cmd[0] = FIFO_INFO;
    cmd[1] = 0x02;
    SI446X_CMD( cmd, 2 );
    SI446X_READ_RESPONSE(cmd, 2);
}



void SI446X_WRITE_TX_FIFO( unsigned char *txbuffer, unsigned char size)
{
	unsigned char cmd[1] = {WRITE_TX_FIFO};
        unsigned char i=0;
	cmd[0] = WRITE_TX_FIFO;

	if (size > 64)
		size = 64;

        SI446X_WAIT_CTS();
          
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_RESET);	//SI_CSN_LOW( );
    SPI_TxRxData(cmd[0]);//SPI_SendData(cmd[0]);//HAL_SPI_Transmit(&hspi2, cmd, 1, 100);	//SPI_ExchangeByte( 0x66 ); 	//WRITE_TX_FIFO = 0x66

    
    while(i<size){SPI_TxRxData(txbuffer[i++]);}//{SPI_SendData(txbuffer[i++]);}//HAL_SPI_Transmit(&hspi2, txbuffer, size, 100);	//while( size -- )    { SPI_ExchangeByte( *txbuffer++ ); }
    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_SET);	//SI_CSN_HIGH( );
}



int SI446X_SET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM, unsigned char proirity )
{
    unsigned char cmd[5];
    cmd[0] = SET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = 1;
    cmd[3] = GROUP_NUM;
    cmd[4] = proirity;
    SI446X_CMD( cmd, 5 );
    return 0;
}


void SI446X_GPIO_CONFIG( unsigned char G0, unsigned char G1, unsigned char G2, unsigned char G3,
		unsigned char IRQ, unsigned char SDO, unsigned char GEN_CONFIG )
{
	unsigned char cmd[10];
    cmd[0] = GPIO_PIN_CFG;
    cmd[1] = G0;
    cmd[2] = G1;
    cmd[3] = G2;
    cmd[4] = G3;
    cmd[5] = IRQ;
    cmd[6] = SDO;
    cmd[7] = GEN_CONFIG;
    SI446X_CMD( cmd, 8 );
    SI446X_READ_RESPONSE( cmd, 8 );
}



unsigned char SI446X_READ_PACKET( unsigned char *buffer , unsigned char size)
{
	unsigned char length = 8, i, cTemp = READ_RX_FIFO;
	SI446X_WAIT_CTS( );
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_RESET);	//SI_CSN_LOW( );
    //SPI_SendData(cTemp);//HAL_SPI_Transmit(&hspi2, &cTemp, 1, 100);	//SPI_ExchangeByte( READ_RX_FIFO );
    SPI_TxRxData(cTemp);

    i=0;
    while(i<size){buffer[i++] = SPI_TxRxData(0x00);}//SPI_ReceiveData();}//HAL_SPI_Receive(&hspi2, buffer, size, 100);

    //SI446X_READ_RESPONSE(buffer, 8);

    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_SET);	//SI_CSN_HIGH( );
    return buffer[0]; 	//i;
}


void SI446X_START_TX( unsigned char channel, unsigned short rx_len)
{
	unsigned char cmd[6];
    cmd[0] = START_TX;
    cmd[1] = channel;
    cmd[2] = 0x30;
    cmd[3] = rx_len>>8;
    cmd[4] = rx_len;
 //   cmd[5] = 0x00;
    SI446X_CMD( cmd, 5 );
    SI446X_WAIT_CTS();
}

void SI446X_START_RX( unsigned char channel, unsigned char condition, unsigned short rx_len,
		unsigned char n_state1, unsigned char n_state2, unsigned char n_state3 )
{
	unsigned char cmd[8];
//    SI446X_RX_FIFO_RESET( );
//    SI446X_TX_FIFO_RESET( );
    cmd[0] = START_RX;
    cmd[1] = channel;
    cmd[2] = condition;
    cmd[3] = rx_len>>8;
    cmd[4] = rx_len;
    cmd[5] = n_state1;
    cmd[6] = n_state2;
    cmd[7] = n_state3;
    SI446X_CMD( cmd, 8 );
    SI446X_WAIT_CTS();
}

/*
void SI446X_READ_FIFO( unsigned char * buffer )
{
	unsigned char cmd[2];
    cmd[0] = FIFO_INFO;
    cmd[1] = 0x00;
    SI446X_CMD( cmd, 2 );
    //HAL_SPI_Receive(&hspi2, buffer, 3, 100);
    SI446X_READ_RESPONSE(buffer, 3);
}
*/

void SI446X_FIFOINFO(unsigned char  *rxCount, unsigned char  *txSpace, unsigned char  resetTx, unsigned char  resetRx)
{
	uint8_t cmd[2], response[3];

	cmd[0] = FIFO_INFO;
	cmd[1] = (resetRx ? 1 << 1 : 0) | (resetTx ? 1 << 0 : 0);

	SI446X_CMD( cmd, 2 );	//_write(command, sizeof(command));

	SI446X_READ_RESPONSE(response, 3);//_pollDataReply(response, sizeof(response));
	if (rxCount)
		*rxCount = response[1];
	if (txSpace)
		*txSpace = response[2];

}



void SI446X_INT_STATUS( unsigned char *buffer )
{
	unsigned char cmd[4];
    cmd[0] = GET_INT_STATUS;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    SI446X_CMD( cmd, 4 );
    SI446X_READ_RESPONSE( buffer, 9 );
}

void SI446X_PART_INFO( unsigned char *buffer )
{
	unsigned char cmd[16];
	cmd[0] = 0x01;										//PART_INFO
	SI446X_CMD(cmd, 1);
	SI446X_READ_RESPONSE( &buffer[0], 9 );

}
void SI446X_POWER_UP(void)
{
	unsigned char cmd[16];

	  cmd[0] = POWER_UP;   //CMD_POWER_UP
	  cmd[1] = 0x01;		// FUNC=1
	  cmd[2] = 0x00;
	  cmd[3] = 0x01;		//30MHz
	  cmd[4] = 0xC9;
	  cmd[5] = 0xC3;
	  cmd[6] = 0x80;

	  SI446X_CMD(cmd, 7);
	  SI446X_WAIT_CTS();

}










char cmdReadFRR(uint8_t number, uint8_t *data)
{
	uint8_t request = 0xFF;

	//NssControl nssLocker(this);

	//SI446X_WAIT_CTS();


	switch (number) {
		case 0:
			request = (uint8_t)FRR_A_READ;
			break;
		case 1:
			request = (uint8_t)FRR_B_READ;
			break;
		case 2:
			request = (uint8_t)FRR_C_READ;
			break;
		case 3:
			request = (uint8_t)FRR_D_READ;
			break;
	}
	//_spi->write_blocking(&request, 1);
	//_spi->read_blocking(data, 1);
    GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_RESET);	//SI_CSN_LOW( );

    //HAL_SPI_Transmit(&hspi2, &request, 1, 100);
    //HAL_SPI_Receive(&hspi2, data, 1, 100);
    
    //SPI_SendData(request);
    //data[0] = SPI_ReceiveData();
    
    SPI_TxRxData(request);
    data[0] = SPI_TxRxData(0x00);
    //data[0] = SPI_TxRxData(0x00);

    GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);//HAL_GPIO_WritePin(GPIOB, SPI2_NSS_Pin, GPIO_PIN_SET); //SI_CSN_HIGH( );

	return 0;
}






