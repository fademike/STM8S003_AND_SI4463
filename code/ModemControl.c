/*
 * ModemControl.c
 *
 *  Created on: 06.04.2020.
 *      Author: fademike
 */


#include "main.h"
#include "ModemControl.h"


#ifdef SI4463

#include "si4463.h"
#include "radio_config_Si4463.h"
#define PACKET_LEN 64
#define PACKET_LEN_CRC 2	      // package Length
#define PACKET_DATALEN 60
#define CMD_LIST_SIZE 5       // any num RAM
#endif


#ifdef nRF24

#include "nRF24.h"
#define PACKET_LEN 32	      // package Length
#define PACKET_LEN_CRC 2	      // package Length
#define PACKET_DATALEN 28   // the size of the data in the package
#define CMD_LIST_SIZE 10    // number of packages

#endif


// struct nrf_pack {  // Pack struct: 
//   unsigned char type;
//   unsigned char len;
//   unsigned char data[PACKET_DATALEN];
//   unsigned char crc[2];
// };

// #ifdef STM8

// inline void memcpy(U8*dst, U8*src, int n){
//   int i;
//   for (i=0;i<n;i++) dst[i] = src[i];
// }

// #else 

#include <string.h>
#include <stdio.h>

// #endif

unsigned char CRC_PacketCalculate(unsigned char * buff);
int CRC_PacketCheck(unsigned char * buff);

#define DataEx(a) (a##_RD != a##_WR)
#define DataExAny(f, a) (f##_RD != a##_WR)  // for send block
#define DataArrayPP(a) if (++a>=CMD_LIST_SIZE) a=0
#define DataArrayMM(a) if (--a<0) a=(CMD_LIST_SIZE-1)


//#define DataArrayPP(a) DataArrayPP_ifnfull(&a##_RD, a##_WR)
//#define DataArrayPP_ifnfull()
void DataArrayPP_ifnfull(int * p_WR, int p_RD){
  int next_WR = * p_WR;
  DataArrayPP(next_WR);
  if (next_WR != p_RD) *p_WR = next_WR;
}

volatile unsigned char pack_counter = 0;

static int ModemControl_status = -1;

unsigned char toTxCmdbuf[CMD_LIST_SIZE][PACKET_LEN];  // buffer for send to RF
int toTxCmd_RD = 0; // Read pointer 
int toTxCmd_WR = 0; // Write pointer

unsigned char toRxCmdbuf[CMD_LIST_SIZE][PACKET_LEN];
int toRxCmd_RD = 0; // Read pointer 
int toRxCmd_WR = 0; // Write pointer


#if SYNCHRO_MODE

//  Variables of modem
volatile int delay_to_wait_answer = 0;
volatile int delay_measure_tx = 0;
volatile int delay_synchro = 0;
volatile int delay_after_send_symbol = 0;
char NeedAnswer = 0;
#endif 


char rfPack_getType(char * buff){
  return buff[0];
}
void rfPack_setType(char * buff, char type){
  buff[0] = type;
}
char rfPack_getLen(char * buff){
  return buff[1]&0x3F;
}
void rfPack_setLen(char * buff, unsigned int len){
	if (len>PACKET_DATALEN) len = PACKET_DATALEN;
  buff[1] = len;
}
char * rfPack_getDataPtr(char * buff, int n){
  return &buff[2+n];
}
void rfPack_setData(char * buff, int n, char data){
  buff[2+n] = data;
}

// Copies the packet to the buffer for sending
void ModemControl_SendPacket(char * buff, unsigned int len)
{
  rfPack_setLen(toTxCmdbuf[toTxCmd_WR], len);
	memcpy(rfPack_getDataPtr(toTxCmdbuf[toTxCmd_WR], 0), buff, len);	// PACKET_DATALEN); // copy packet for send
	DataArrayPP_ifnfull(&toTxCmd_WR, toTxCmd_RD);//DataArrayPP(toTxCmd_WR);
}

volatile unsigned int ptr_SendSymbol=0; // pointer to fill in the block package
void ModemControl_SendSymbol_Complete(void)
{
  rfPack_setLen(toTxCmdbuf[toTxCmd_WR], ptr_SendSymbol);//  toTxCmdbuf[toTxCmd_WR][1] = ptr_SendSymbol&0x3F;
	DataArrayPP_ifnfull(&toTxCmd_WR, toTxCmd_RD);  // increase painter for send
	ptr_SendSymbol=0;
}
void ModemControl_SendSymbol(char data)
{
  rfPack_setData(toTxCmdbuf[toTxCmd_WR], ptr_SendSymbol++, data);
	if (ptr_SendSymbol>=PACKET_DATALEN){ // if the buffer is full
		ModemControl_SendSymbol_Complete();
	}
	delay_after_send_symbol=0;
}
// If there is a package, puts the package in the pointer (buf)
// Returned len pack if there is packet. 0 no packet
int ModemControl_GetPacket(char * buf){
    if (!DataEx(toRxCmd)) return 0;

    unsigned int len = rfPack_getLen(toRxCmdbuf[toRxCmd_RD]);
    if (len > PACKET_DATALEN) len = 0;//PACKET_DATALEN;
    memcpy(buf, rfPack_getDataPtr(toRxCmdbuf[toRxCmd_RD], 0), len);//PACKET_DATALEN);
    //buf = rfPack_getDataPtr(toRxCmdbuf[toRxCmd_RD], 0);//&toRxCmdbuf[toRxCmd_RD][2];
    DataArrayPP(toRxCmd_RD);
    return len;
}
// Reads received packets and outputs one byte to pointer (buf)
// Returned 1 if there is data. 0 no data
int ModemControl_GetByte(char * buf){     //FIXME !!!
  //struct nrf_pack * pack =  (struct nrf_pack *)toRxCmdbuf[toRxCmd_RD];
    static int pos = 0;
    int read = 0; // flag of return data
    if (!DataEx(toRxCmd)) {pos=0; return 0;}

    char len = rfPack_getLen(toRxCmdbuf[toRxCmd_RD]);//pack->len&0x3F;
    if (len > PACKET_DATALEN) len=PACKET_LEN;
    if (len>pos){
        *buf = *rfPack_getDataPtr(toRxCmdbuf[toRxCmd_RD], pos);//toRxCmdbuf[toRxCmd_RD][2+pos]; // 2+pos, because datain pack start from second byte
        pos++;
        read = 1;
    }
    if (pos >= len){  // all bytes in this package geting 
        pos = 0;
        DataArrayPP(toRxCmd_RD);  //increase pointer
    }
    return read;
}

// return calculated crc 
unsigned char CRC_PacketCalculate(unsigned char * buff){
	unsigned char mCRC = 87;  // start byte for calculate my crc
	int x=0;
	for (x=0;x<(PACKET_LEN-PACKET_LEN_CRC);x++)mCRC += buff[x]*3; // algorithm of calculation
	return mCRC;
}
// write calculated crc in pack
void CRC_PacketSet(unsigned char * buff){
	buff[PACKET_LEN-PACKET_LEN_CRC] = CRC_PacketCalculate(buff);
}
// check crc pack
int CRC_PacketCheck(unsigned char * buff){
	return  (CRC_PacketCalculate(buff) == buff[PACKET_LEN-PACKET_LEN_CRC]);
}

int ModemControl_getStatus(void){
	return ModemControl_status;
}

int ModemControl_init(void){

  ModemControl_status = RFinit();			//Set Settings Config to Si4463

  //if (ModemControl_status >= 0) printf("rf init\n\r");
  
  return ModemControl_status;
}

void MC_setVar(char t_NeedAnswer, int t_delay_to_wait_answer);
void MC_setVar(char t_NeedAnswer, int t_delay_to_wait_answer){
  NeedAnswer = t_NeedAnswer;
  delay_to_wait_answer = t_delay_to_wait_answer;
}

int ModemControl_Read(void){

  unsigned char rbuff[PACKET_LEN];

  int ret = RFread(rbuff, PACKET_LEN);
  
  if (ret > 0){

    char type = rfPack_getType(rbuff);

 #if SYNCHRO_MODE

    if (type == PACKET_NONE )         MC_setVar(0,0);
    else if (type == PACKET_ASK )     MC_setVar(PACKET_ANSWER, 0);
    else if (type == PACKET_ANSWER )  MC_setVar(0,0);
    else if (type == PACKET_DATA)     MC_setVar(PACKET_ANSWER, 0);
    else if (type == PACKET_DATA_PART)MC_setVar(0,SYNCHRO_TWAIT);
 #endif
 
    if ((type == PACKET_DATA) || (type == PACKET_DATA_PART)){         // if Packet is PACKET_DATA
      char len = rfPack_getLen(rbuff);  // datalen of package
      if (len){ // if no zero len
        if 	(1){//(CRC_PacketCheck(rbuff)){ //TODO !!!
          memcpy(toRxCmdbuf[toRxCmd_WR], rbuff, len+2); // copy type, len, data. (without crc)
          DataArrayPP(toRxCmd_WR);
        }
      }
    }
    return 1;//continue;
  }
  return 0;
}

void ModemControl_timeUpdate(void){
  //TODO
#if SYNCHRO_MODE
  static unsigned long localTime_ms = 0;
  unsigned long realTime_ms = GetTime_ms();

  unsigned long dt_ms = realTime_ms - localTime_ms;
  if (localTime_ms > realTime_ms) dt_ms = 0;
  localTime_ms = realTime_ms;
  if (dt_ms<1) return;

  if (delay_to_wait_answer>0) delay_to_wait_answer -= dt_ms;
  if (delay_measure_tx<100) delay_measure_tx += dt_ms; //limit calculate tx is 100 ms
  if (delay_synchro>0) delay_synchro -= dt_ms;
  if (delay_after_send_symbol<100) delay_after_send_symbol += dt_ms;

#endif
}


void ModemControl_Send(unsigned char * buf){
  test_set(1);
  ModemControl_timeUpdate(); //update timer
  delay_measure_tx = 0;  // reset timer to caltulate time tx pack

  RFwrite(buf, PACKET_LEN);                                         // TX Data

#if SYNCHRO_MODE
  ModemControl_timeUpdate(); //update timer
  if (delay_measure_tx <= 0)delay_measure_tx = 1;  // time tx minimum 1ms
  if (SYNCHRO_MODE) delay_to_wait_answer = delay_measure_tx+SYNCHRO_TWAIT;  //50; //Set timer between send the packets
  //delay_synchro = SYNCHRO_TIME;    //else delay_to_wait_answer = 10;	//delay_to_wait_answer = 10;  //50;
#endif
  test_set(0);
}

int ModemControl_Loop(void)
{
  if (ModemControl_status < 0) {ModemControl_init(); return -1;}
  ModemControl_timeUpdate();

  if (ModemControl_Read() != 0) return 1;

  if (ptr_SendSymbol>0) { //if to have symbols to send
    if (delay_after_send_symbol > 2) ModemControl_SendSymbol_Complete();  // prepare to tx, when no new data 2ms
  }

  //if (SPI_Read_Reg(CD) != 0) {return;}

#if SYNCHRO_MODE
  if (delay_to_wait_answer > 0){return 0;}      //The timer between the packets being sent. Otherwise, heap data was not accepted.
#endif

  #define SEND_PACKETBLOCK 0

  if (DataEx(toTxCmd)){
#if SEND_PACKETBLOCK
    do{
      int future_RD = toTxCmd_RD;
      DataArrayPP(future_RD);
      if (DataExAny(future, toTxCmd)) toTxCmdbuf[toTxCmd_RD][0] = PACKET_DATA_PART;
      else  toTxCmdbuf[toTxCmd_RD][0] = PACKET_DATA;
      
#else
      toTxCmdbuf[toTxCmd_RD][0] = PACKET_DATA;
#endif
      //toTxCmdbuf[toTxCmd_RD][1] = (PACKET_DATALEN)&0x3F;
      //toTxCmdbuf[toTxCmd_RD][63] = pack_counter++;
      ModemControl_Send(toTxCmdbuf[toTxCmd_RD]);

      DataArrayPP(toTxCmd_RD);
#if SEND_PACKETBLOCK
    } while(DataEx(toTxCmd));
#endif

#if SYNCHRO_MODE
    delay_synchro = SYNCHRO_TIME;    //else delay_to_wait_answer = 10;	//delay_to_wait_answer = 10;  //50;
    NeedAnswer=0;
#endif
  }

#if SYNCHRO_MODE

  if (((NeedAnswer>0) || ((delay_synchro>0) && (delay_to_wait_answer <= 0)))){      //Maintain communication by sync pulses.

    //Delay_ms(delay_measure_tx_middle);                                  //So as not to foul the broadcast.
    unsigned char wbuff[64];
    if (NeedAnswer) {wbuff[0] = NeedAnswer; NeedAnswer=0;}             // Pulse is Answer
    else if (delay_synchro>0) wbuff[0] = PACKET_ASK;                    // Pulse is Ask
    else wbuff[0] = PACKET_NONE;                                    // Pulse is None
    ModemControl_Send(wbuff);
  }
#endif


  return 0;
}
