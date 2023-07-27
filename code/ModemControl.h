/*
 * ModemControl.h
 *
 *  Created on: 06.04.2020.
 *      Author: fademike
 */


typedef enum  {
	STATE_IDLE 				= 0,
	STATE_RX				= 1,
	STATE_TX				= 2
} STATE_TRANSIEVER;

typedef enum  {
	PACKET_NONE 		= 0,
	PACKET_DATA			= 1,
	PACKET_ASK			= 2,
	PACKET_ANSWER		= 3,
	PACKET_DATA_PART	= 4,
} PACKET_TYPE;



#define SYNCHRO_MODE 1	// 1- synchronization mode; 0- simple mode. More in stm8+si4463 project

#if SYNCHRO_MODE

#define SYNCHRO_TIME 500                                                        // sync time
#define SYNCHRO_TWAIT 20//6*4

#endif

int ModemControl_init(void);

int ModemControl_getStatus(void);

int ModemControl_Loop(void);	// return 1 when rx data
int ModemControl_ReadOnly(void);

void ModemControl_SendPacket(char * buff, unsigned int len);
void ModemControl_SendSymbol(char buff);

int ModemControl_GetPacket(char * buf);
int ModemControl_GetByte(char * buf);
//



