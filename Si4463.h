/*
 * Si4463.h
 *
 *  Created on: 16 июля 2016 г.
 *      Author: NASA
 */

#ifndef SRC_SI4463_H_
#define SRC_SI4463_H_


#include "si446x_defs.h"








void SI446X_WAIT_CTS( void );
void SI446X_CMD(const unsigned char *cmd, unsigned char cmdsize );
void SI446X_READ_RESPONSE( unsigned char *buffer, unsigned char size );
void SI446X_TX_FIFO_RESET( void );
void SI446X_RX_FIFO_RESET( void );
//void SI446X_SEND_PACKET( unsigned char *txbuffer, unsigned char size, unsigned char channel, unsigned char condition );
//void SI446X_SEND_PACKET( unsigned char *txbuffer, unsigned char size);
void SI446X_WRITE_TX_FIFO( unsigned char *txbuffer, unsigned char size);
int SI446X_SET_PROPERTY_1( SI446X_PROPERTY GROUP_NUM, unsigned char proirity );
void SI446X_GPIO_CONFIG( unsigned char G0, unsigned char G1, unsigned char G2, unsigned char G3,
		unsigned char IRQ, unsigned char SDO, unsigned char GEN_CONFIG );
void SI446X_START_TX( unsigned char channel, unsigned short rx_len);
unsigned char SI446X_READ_PACKET( unsigned char *buffer , unsigned char size);
void SI446X_START_RX( unsigned char channel, unsigned char condition, unsigned short rx_len,
		unsigned char n_state1, unsigned char n_state2, unsigned char n_state3 );
//void SI446X_READ_FIFO( unsigned char * buffer );
void SI446X_FIFOINFO(unsigned char  *rxCount, unsigned char  *txSpace, unsigned char  resetTx, unsigned char  resetRx);
void SI446X_INT_STATUS( unsigned char *buffer );
void SI446X_PART_INFO( unsigned char *buffer );
void SI446X_POWER_UP(void);

char cmdReadFRR(uint8_t number, uint8_t *data);


#endif /* SRC_SI4463_H_ */
