/*
 * RfTransceiver.h
 *
 *  Created on: 16 июля 2016 г.
 *      Author: NASA
 */

#ifndef INC_RFTRANSCEIVER_H_
#define INC_RFTRANSCEIVER_H_


unsigned char getStatus(void);
uint8_t changeState(uint8_t state);
uint32_t RFread(uint8_t *data, uint32_t length);
uint32_t RFwrite(unsigned char *data, uint8_t length);

void MYinit(void);

#endif /* INC_RFTRANSCEIVER_H_ */
