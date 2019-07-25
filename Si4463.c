/*
 * Si4463.c
 *
 *  Created on: 16 июля 2016 г.
 *      Author: NASA
 */


#include "si446x_defs.h"
#include "Si4463.h"
#include "radio_config_Si4463.h"

#ifdef STM8
#include "stm8s.h"
#else
//#include "main.h"
#include "stm32f4xx_hal.h"

extern SPI_HandleTypeDef hspi1;

#endif


//extern SPI_HandleTypeDef hspi2;


uint8_t SPI_TxRxData(uint8_t request){
  uint8_t answer=0;
  //STM8
#ifdef STM8
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    SPI_SendData(request);//SPI_SendData(cTemp);//while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET){};//HAL_SPI_Transmit(&hspi2, &cTemp, 1, 100);
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){}; answer = SPI_ReceiveData();
    request = answer;                                                                                                                                   //TODO
    //while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
#else
    HAL_SPI_TransmitReceive(&hspi1, &request, &answer, 1, 100);
#endif

    return answer;
}

void SPI_Set_NSS(int set)
{
#ifdef STM8
	if(set == 0 ) GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);
	else GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);

#else
	if(set == 0 ) HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);//GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);
	else HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);	// GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2);
#endif
}


#define READ_CMD_BUFF 0x44

void SI446X_WAIT_CTS( void )
{
	uint8_t cTemp = 0x44;
	uint8_t cts;

    do
    {
    	SPI_Set_NSS(0);

        SPI_TxRxData(cTemp);
        cts = SPI_TxRxData(0x00);
        
        SPI_Set_NSS(1);

    } while( cts != 0xFF );
    
    
}




void SI446X_CMD(uint8_t *cmd, uint8_t cmdsize )
{
#ifdef STM8
    uint8_t i=0;
#endif
    SI446X_WAIT_CTS( );
    SPI_Set_NSS(0);

#ifdef STM8
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    while(i<cmdsize){
    	SPI_SendData(cmd[i]);
    	while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){};
    	SPI_ReceiveData();
    	i++;
    	while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    }
#else
    HAL_SPI_Transmit(&hspi1, cmd, cmdsize, 100);
#endif
    SPI_Set_NSS(1);
}


void SI446X_READ_RESPONSE( uint8_t *buffer, uint8_t size )
{
	uint8_t cTemp = 0x44;
    uint8_t i=0;
	//uint8_t cts = 0x00;
    for (i=0;i<size;i++)buffer[i] = 0x00;
    i=0;
    
    SI446X_WAIT_CTS( );
    SPI_Set_NSS(0);
    
#ifdef STM8
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};
    SPI_SendData(cTemp);//while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET){};//HAL_SPI_Transmit(&hspi2, &cTemp, 1, 100);
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){};
    SPI_ReceiveData();
    while(SPI_GetFlagStatus(SPI_FLAG_BSY) != RESET){};

    while(i<size){SPI_SendData(0x00);while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET){};

      buffer[i++] = SPI_ReceiveData();}//HAL_SPI_Receive(&hspi2, buffer, size, 100);
#else
    HAL_SPI_Transmit(&hspi1, &cTemp, 1, 100);
    HAL_SPI_Receive(&hspi1, buffer, size, 100);
#endif
    SPI_Set_NSS(1);
}


void SI446X_TX_FIFO_RESET( void )
{
	uint8_t cmd[2+1];
    cmd[0] = FIFO_INFO;		//FIFO_INFO = 0x15
    cmd[1] = 0x01;
    SI446X_CMD( cmd, 2 );
    SI446X_READ_RESPONSE(cmd, 2);
}


void SI446X_RX_FIFO_RESET( void )
{
	uint8_t cmd[2+1];
    cmd[0] = FIFO_INFO;
    cmd[1] = 0x02;
    SI446X_CMD( cmd, 2 );
    SI446X_READ_RESPONSE(cmd, 2);
}



void SI446X_WRITE_TX_FIFO( uint8_t *txbuffer, uint8_t size)
{
	uint8_t cmd[1] = {WRITE_TX_FIFO};
        uint8_t i=0;
	cmd[0] = WRITE_TX_FIFO;

	if (size > 64)
		size = 64;

        SI446X_WAIT_CTS();
          
    SPI_Set_NSS(0);
    SPI_TxRxData(cmd[0]);

    
    while(i<size){SPI_TxRxData(txbuffer[i++]);}
    SPI_Set_NSS(1);
}



int SI446X_SET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM, uint8_t proirity )
{
    uint8_t cmd[5];
    cmd[0] = SET_PROPERTY;
    cmd[1] = GROUP_NUM>>8;
    cmd[2] = 1;
    cmd[3] = GROUP_NUM;
    cmd[4] = proirity;
    SI446X_CMD( cmd, 5 );
    return 0;
}


void SI446X_GPIO_CONFIG( uint8_t G0, uint8_t G1, uint8_t G2, uint8_t G3,
		uint8_t IRQ, uint8_t SDO, uint8_t GEN_CONFIG )
{
	uint8_t cmd[10];
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



uint8_t SI446X_READ_PACKET( uint8_t *buffer , uint8_t size)
{
	uint8_t i, cTemp = READ_RX_FIFO;
	SI446X_WAIT_CTS( );
	SPI_Set_NSS(0);

    SPI_TxRxData(cTemp);

    i=0;
    while(i<size){buffer[i++] = SPI_TxRxData(0);}


    SPI_Set_NSS(1);
    return buffer[0]; 	//i;
}


void SI446X_START_TX( uint8_t channel, uint16_t rx_len)
{
	uint8_t cmd[6];
    cmd[0] = START_TX;
    cmd[1] = channel;
    cmd[2] = 0x30;
    cmd[3] = rx_len>>8;
    cmd[4] = rx_len;
 //   cmd[5] = 0x00;
    SI446X_CMD( cmd, 5 );
    SI446X_WAIT_CTS();
}

void SI446X_START_RX( uint8_t channel, uint8_t condition, uint16_t rx_len,
		uint8_t n_state1, uint8_t n_state2, uint8_t n_state3 )
{
	uint8_t cmd[8];
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
void SI446X_READ_FIFO( uint8_t * buffer )
{
	uint8_t cmd[2];
    cmd[0] = FIFO_INFO;
    cmd[1] = 0x00;
    SI446X_CMD( cmd, 2 );
    //HAL_SPI_Receive(&hspi2, buffer, 3, 100);
    SI446X_READ_RESPONSE(buffer, 3);
}
*/

void SI446X_FIFOINFO(uint8_t  *rxCount, uint8_t  *txSpace, uint8_t  resetTx, uint8_t  resetRx)
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



void SI446X_INT_STATUS( uint8_t *buffer )
{
	uint8_t cmd[4];
    cmd[0] = GET_INT_STATUS;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;
    SI446X_CMD( cmd, 4 );
    SI446X_READ_RESPONSE( buffer, 9 );
}

void SI446X_PART_INFO( uint8_t *buffer )
{
	uint8_t cmd[16];
	cmd[0] = 0x01;										//PART_INFO
	SI446X_CMD(cmd, 1);
	SI446X_READ_RESPONSE( &buffer[0], 9 );

}
void SI446X_POWER_UP(void)
{
	uint8_t cmd[16];

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

	SPI_Set_NSS(0);
    SPI_TxRxData(request);
    data[0] = SPI_TxRxData(0x00);

    SPI_Set_NSS(1);

	return 0;
}



int8_t getLevel(void)
{
	uint8_t level = 0;

	cmdReadFRR(0, &level);
	//return -((int8_t)(127 - level / 2));
	return level;
}

#define REGISTRS 1

uint8_t getStatus(void)
{
#if REGISTRS
	uint8_t status;
	cmdReadFRR(1, (uint8_t *)&status);
	return status;
#else
	uint8_t buffer[9];
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
	uint8_t bufferSize = length;//64;
	//SI446X_FIFOINFO(&bufferSize, NULL, 0, 0);
	SI446X_READ_PACKET(data , bufferSize);
	return bufferSize;

}



uint32_t RFwrite(uint8_t *data, uint8_t length)
{

	//SI446X_WRITE_TX_FIFO(data, length);//packetLength);
	//return length;

        SI446X_WRITE_TX_FIFO(data, length);

        changeState(STATE_TX);
        while((getStatus()&0x20) == 0x00){};

        changeState(STATE_RX);

        return 0;
}


uint8_t config_table[] = RADIO_CONFIGURATION_DATA_ARRAY;


void RFinit(void)
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



void setFrequency(int32_t f)
{
	uint8_t data_cmd[] = {RF_FREQ_CONTROL_INTE_8};
	  //unsigned long Constant = 1<<19;
	uint8_t Array_RF_MODEM_CLKGEN_BAND_1[] = {RF_MODEM_CLKGEN_BAND_1};
	uint32_t OUTDIV = Array_RF_MODEM_CLKGEN_BAND_1[4]&0x7;
	  if (OUTDIV == 0) OUTDIV=4;
	  else if (OUTDIV == 1) OUTDIV=6;
	  else if (OUTDIV == 2) OUTDIV=8;
	  else if (OUTDIV == 3) OUTDIV=12;
	  else if (OUTDIV == 4) OUTDIV=16;
	  else OUTDIV=24;

	  uint8_t NPRESC = 0;
	  if(Array_RF_MODEM_CLKGEN_BAND_1[4]&0x8) NPRESC = 2;
	  else NPRESC = 4;

	  uint32_t freq_xo = RADIO_CONFIGURATION_DATA_RADIO_XO_FREQ;

	  uint32_t d = 524288;//1<<19;
	  uint32_t C = NPRESC*freq_xo / OUTDIV;

	  uint32_t result = f%C;
	  uint32_t inte = (f/C)-1;

	  uint32_t frac_float = ((float)d/(float)C)* ((float)C + (float)result);

	  uint8_t frac_1 = (frac_float>>16)&0xFF;
	  uint8_t frac_2 = (frac_float>>8)&0xFF;
	  uint8_t frac_3 = (frac_float>>0)&0xFF;

	  data_cmd[4] = inte&0xFF;

	  data_cmd[5] = frac_1;
	  data_cmd[6] = frac_2;
	  data_cmd[7] = frac_3;

	  SI446X_CMD(&data_cmd[0], 0x0C );

}







