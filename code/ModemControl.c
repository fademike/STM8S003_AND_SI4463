/*
 * ModemControl.c
 *
 *  Created on: 06 ���. 2020 �.
 *      Author: NASA
 */

//#include <string.h>	// for memcpy

#include "main.h"

#include "ModemControl.h"
//#include "mavlink_handler.h"
//#include "system.h"
// #include "spi.h"

//#define PACKET_DATALEN 0

// #ifdef LINUX
// #include <string.h>
// #endif

#ifdef SI4463
#include "si4463.h"
#include "radio_config_Si4463.h"

//#include "spi.h"

#define PACKET_LEN 64
#define PACKET_LEN_CRC 2	      // package Length
#define PACKET_DATALEN 60
//#define CMD_BUF_SIZE 64     // max buffer nRF32L01
#define CMD_LIST_SIZE 5    // any num RAM
#endif


#ifdef nRF24

#include "nRF24.h"
#define PACKET_DATALEN 28

#define PACKET_LEN 32	      // package Length
#define PACKET_LEN_CRC 2	      // package Length
#define PACKET_DATALEN 28   // the size of the data in the package
#define CMD_LIST_SIZE 10    // number of packages


#endif

#ifdef STM8

struct nrf_pack {
  unsigned char type;
  unsigned char len;
  unsigned char data[PACKET_DATALEN];
  unsigned char crc[2];
};

inline void memcpy(U8*dst, U8*src, int n){
  int i;
  for (i=0;i<n;i++) dst[i] = src[i];
}


#else 

#pragma pack(push, 1)
struct nrf_pack {
  unsigned char type;
  unsigned char len;
  unsigned char data[PACKET_DATALEN];
  unsigned char crc[2];
};
#pragma pack(pop)

#include <string.h>

#endif

#define DataEx(a) (a##_RD != a##_WR)
#define DataExAny(f, a) (f##_RD != a##_WR)  // for send block
#define DataArrayPP(a) if (++a>=CMD_LIST_SIZE) a=0
#define DataArrayMM(a) if (--a<0) a=(CMD_LIST_SIZE-1)


//#define DataArrayPP(a) DataArrayPP_ifnfull(&a##_RD, a##_WR)
// #define DataArrayPP_ifnfull()
void DataArrayPP_ifnfull(int * p_WR, int p_RD){
  int next_WR = * p_WR;
  DataArrayPP(next_WR);//if (++next_WR>=CMD_LIST_SIZE) next_WR=0;  //DataArrayPP(next_WR);
  if (next_WR != p_RD) *p_WR = next_WR;
}

volatile unsigned char pack_counter = 0;

static int ModemControl_status = -1;

unsigned char toSendCmdbuf[CMD_LIST_SIZE][PACKET_LEN];  // buffer for send to RF
int toSendCmd_RD; // Read pointer 
int toSendCmd_WR; // Write pointer

unsigned char toRxCmdbuf[CMD_LIST_SIZE][PACKET_LEN];
int toRxCmd_RD; // Read pointer 
int toRxCmd_WR; // Write pointer

////////////////
//circular buffer for receive commands
//#define READCMDSTREAM_ARRAY 10

// unsigned char ReadCommandStreamArray[READCMDSTREAM_ARRAY][100];
// unsigned char ReadCommandStreamArrayCnt[READCMDSTREAM_ARRAY];
// int ReadCommandStreamArray_rd=0;
// int ReadCommandStreamArray_wr=0;

#if SYNCHRO_MODE

//  Variables of modem
//volatile int time_delay=0;
//volatile int time_last_tx=0;
volatile int delay_to_wait_answer=0;
volatile int delay_measure_tx=0;
volatile int delay_synchro=0;
volatile int delay_after_send_symbol=0;
char NeedAnswer=0;
//int delay_measure_tx_middle=0;
#endif 

//DELETE									//circular buffer for send out by RF
//#define UART_BUF_SIZE 255			// buffer lenght
char UartRxBuffer[UART_BUF_SIZE];	// buffer
volatile int UartRxBuffer_rd = 0;	// read array
volatile int UartRxBuffer_wr = 0;	//	write array


//#define BUFFER_TX_RF 64
char BufferTxRf[UART_BUF_SIZE];	// buffer
volatile int BufferTxRf_len = 0;	// read array
// char BufferTxTextRf[UART_BUF_SIZE];	// buffer
// volatile int BufferTxTextRf_len = 0;	// read array



//unsigned char rbuff[PACKET_LEN],  wbuff[PACKET_LEN];	// in/out buffer of modem



#define MODEM_PINCONTROL 0	// For debug pin output





// Copies the packet to the buffer for sending
void ModemControl_SendPacket(const char * buff, unsigned int len)
{
	if (len>PACKET_DATALEN) len = PACKET_DATALEN;
	toSendCmdbuf[toSendCmd_WR][1] = len&0x3F;
	memcpy(&toSendCmdbuf[toSendCmd_WR][2], buff, len);	// PACKET_DATALEN); // copy packet for send
	DataArrayPP_ifnfull(&toSendCmd_WR, toSendCmd_RD);//DataArrayPP(toSendCmd_WR);
}

volatile unsigned int ptr_SendSymbol=0; // pointer to fill in the block package
void ModemControl_SendSymbol_Complete(void)
{
    toSendCmdbuf[toSendCmd_WR][1] = ptr_SendSymbol&0x3F;
	DataArrayPP_ifnfull(&toSendCmd_WR, toSendCmd_RD);//DataArrayPP(toSendCmd_WR);
	//DataArrayPP(toSendCmd_WR);  // increase painter for send
	ptr_SendSymbol=0;
}
void ModemControl_SendSymbol(char data)
{
//	char * buff = &toSendCmdbuf[toSendCmd_WR][2]; // buffer of the filled package
//	buff[ptr_SendSymbol++] = data; // add new data to buffer
	toSendCmdbuf[toSendCmd_WR][2+(ptr_SendSymbol++)] = data;
	if (ptr_SendSymbol>=PACKET_DATALEN){ // if the buffer is full
		ModemControl_SendSymbol_Complete();
	}
	delay_after_send_symbol=0;
	// static int ptr=0; // pointer to fill in the block package
	// static char buff[PACKET_DATALEN]; // buffer of the filled package
	// buff[ptr++] = data; // add new data to buffer
	// if (ptr>=PACKET_DATALEN){ // if the buffer is full
	// 	memcpy(&toSendCmdbuf[toSendCmd_WR][2], buff, PACKET_DATALEN); // copy full buffer for send
	// 	DataArrayPP(toSendCmd_WR);
	// 	ptr=0;
	// }
}
// If there is a package, puts the package in the pointer (buf)
// Returned len pack if there is packet. 0 no packet
int ModemControl_GetPacket(char * buf){
    if (!DataEx(toRxCmd)) return 0;

    unsigned int len = toRxCmdbuf[toRxCmd_RD][1]&0x3F;
    if (len > PACKET_DATALEN) len = 0;//PACKET_DATALEN;
    //memcpy(buf, &toRxCmdbuf[toRxCmd_RD][2], len);//PACKET_DATALEN);
    buf = &toRxCmdbuf[toRxCmd_RD][2];
    DataArrayPP(toRxCmd_RD);
    return len;
}
char * ModemControl_GetPacketPtr(int * len){
  char * buf;
    if (!DataEx(toRxCmd)) return 0;

    * len = toRxCmdbuf[toRxCmd_RD][1]&0x3F;
    if (*len > PACKET_DATALEN) *len = 0;//PACKET_DATALEN;
    //memcpy(buf, &toRxCmdbuf[toRxCmd_RD][2], len);//PACKET_DATALEN);
    buf = &toRxCmdbuf[toRxCmd_RD][2];
    DataArrayPP(toRxCmd_RD);
    return buf;
}
// Reads received packets and outputs one byte to pointer (buf)
// Returned 1 if there is data. 0 no data
int ModemControl_GetByte(char * buf){     //FIXME !!!
  struct nrf_pack * pack =  (struct nrf_pack *)toRxCmdbuf[toRxCmd_RD];
    static int pos = 0;
    int read = 0; // flag of return data
    if (!DataEx(toRxCmd)) {pos=0; return 0;}

    char len = pack->len&0x3F;
    //int len = toRxCmdbuf[toRxCmd_RD][1]&0x3F;
    if (len > PACKET_DATALEN) len=PACKET_LEN;
    if (len>pos){
        *buf = toRxCmdbuf[toRxCmd_RD][2+pos]; // 2+pos, because datain pack start from second byte
        pos++;
        read = 1;
    }
    if (pos >= len){  // all bytes in this package geting 
        pos = 0;
        DataArrayPP(toRxCmd_RD);  //increase pointer
    }
    return read;
}


void CRC_PacketCalculate(unsigned char * buff){
	unsigned char mCRC = 87;
	int x=0;
	for (x=0;x<(PACKET_LEN-PACKET_LEN_CRC);x++)mCRC += buff[x]*3; // algorithm of calculation
	buff[PACKET_LEN-PACKET_LEN_CRC] = mCRC;
}

int CRC_PacketCheck(unsigned char * buff){
	unsigned char mCRC = 87;  // start byte for calculate my crc
	int x=0;
	for (x=0;x<(PACKET_LEN-PACKET_LEN_CRC);x++)mCRC += buff[x]*3; // algorithm of calculation
	return  (buff[PACKET_LEN-PACKET_LEN_CRC] == mCRC);
}

int ModemControl_getStatus(void){
	return ModemControl_status;
}

int ModemControl_init(void){


#ifdef SI4463


	    ModemControl_status = RFinit();			//Set Settings Config to Si4463

      if (ModemControl_status < 0) return -1;
	    //setFrequency(433.3*1000*1000);
	    setPower(0x20);//(0x08);	//(0x7F);

	    SI446X_FIFOINFO(0, 0, 1, 1);		// BUFFER CLEAR

	    changeState(STATE_RX);			// Set state to RX

	    Printf("si4463 init\n\r");


	#if 0	// carrier mode
			  setCarrier(1);
			  char wbuff[64];
			  RFwrite(wbuff, PACKET_LEN);

			  while(1){RFwrite(wbuff, PACKET_LEN);};
	#endif

#endif

#ifdef nRF24


	ModemControl_status = nRF24_init();
	if (ModemControl_status < 0) printf("nRF_init false\n\r");
	else printf("nRF_init OK\n\r");
#endif
	return ModemControl_status;
}


int ModemControl_Read(void){

#ifdef SI4463

  unsigned char rbuff[PACKET_LEN];

  int ret = RFread(rbuff, PACKET_LEN);
  
  struct nrf_pack * pack = (struct nrf_pack *) rbuff;

  if (ret > 0){

 #if SYNCHRO_MODE
      if (pack->type == PACKET_NONE ){
        NeedAnswer = 0; delay_to_wait_answer = 0;
      }
      else if (pack->type == PACKET_ASK ){
        NeedAnswer = PACKET_ANSWER; delay_to_wait_answer = 0;
      }
      else if (pack->type == PACKET_ANSWER ){
        NeedAnswer = 0; delay_to_wait_answer = 0;
      }
      else
 #endif
      if ((pack->type == PACKET_DATA) || (pack->type == PACKET_DATA_PACK)){                                   // if Packet is PACKET_DATA
 #if SYNCHRO_MODE
        if (pack->type == PACKET_DATA) {NeedAnswer = PACKET_ANSWER; delay_to_wait_answer = 0;}	// if PACKET_DATA
        else {NeedAnswer = 0; delay_to_wait_answer = SYNCHRO_TWAIT;}							// if PACKET_DATA_PACK
 #endif
        char len = pack->len&0x3F; // datalen of package
        if (len){ // if no zero len
          if 	(1){//(CRC_PacketCheck(rbuff)){ //TODO !!!
            //Printf("[RX]");
            //memcpy(toRxCmdbuf[toRxCmd_WR], rbuff, PACKET_LEN);
            memcpy(toRxCmdbuf[toRxCmd_WR], rbuff, len+2);//PACKET_LEN);
            DataArrayPP(toRxCmd_WR);

            // static unsigned char rx_pack = 0;
            // if (rbuff[63] !=rx_pack){rx_pack = rbuff[63]; test_set(1);}//Printf("[E]");}
            // else {test_set(0);}
          	// rx_pack++;
          }
        }
      }


    return 1;//continue;
  }
  return 0;

#endif

#ifdef nRF24
  unsigned char status = SPI_Read_Reg(STATUS);

 	if(status & 0x40){         // if there is new package

 		SPI_RW_Reg(WRITE_REG_NRF24L01 + STATUS, 0x40);

 		unsigned char fifo_status = 0;
 		do{
 		  unsigned char rbuff[PACKET_LEN];
      struct nrf_pack * pack = (struct nrf_pack *) rbuff;

     	SPI_Read_Buf(RD_RX_PLOAD,rbuff,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer

 #if SYNCHRO_MODE
      if (pack->type == PACKET_NONE ){
        NeedAnswer = 0; delay_to_wait_answer = 0;
      }
      else if (pack->type == PACKET_ASK ){
        NeedAnswer = PACKET_ANSWER; delay_to_wait_answer = 0;
      }
      else if (pack->type == PACKET_ANSWER ){
        NeedAnswer = 0; delay_to_wait_answer = 0;
      }
      else
 #endif
      if ((pack->type == PACKET_DATA) || (pack->type == PACKET_DATA_PACK)){                                   // if Packet is PACKET_DATA

 #if SYNCHRO_MODE
        if (pack->type == PACKET_DATA) {NeedAnswer = PACKET_ANSWER; delay_to_wait_answer = 0;}
        else {NeedAnswer = 0; delay_to_wait_answer = SYNCHRO_TWAIT;}
 #endif
        char len = pack->len&0x3F; // datalen of package
        if (len){ // if no zero len
          if 	(CRC_PacketCheck(rbuff)){
            memcpy(toRxCmdbuf[toRxCmd_WR], rbuff, PACKET_LEN); 
            DataArrayPP(toRxCmd_WR);
          }
        }
      }

			fifo_status=SPI_RW_Reg(FIFO_STATUS, 0);
		}while((fifo_status&0x01) != 0x01);

 		RX_Mode(); // Switch to RX mode

    return 1;
  }

 	if(status&TX_DS)
 	{
 		SPI_RW_Reg(WRITE_REG_NRF24L01 + STATUS, TX_DS);  	// Reset bit TX_DS
 	}
 	if(status&MAX_RT)
 	{
 		SPI_RW_Reg(WRITE_REG_NRF24L01 + STATUS, MAX_RT); 	// Reset bit MAX_RT
 	}
  RX_Mode(); //FIXME
  return 0;
#endif
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
  //if (delay_measure_tx<100) delay_measure_tx += dt_ms; //limit calculate tx is 100 ms
  if (delay_synchro>0) delay_synchro -= dt_ms;
  if (delay_after_send_symbol<100) delay_after_send_symbol++;

  // if (delay_to_wait_answer>0) delay_to_wait_answer -= dt_ms;
  // if (delay_measure_tx<100) delay_measure_tx += dt_ms; //limit calculate tx is 100 ms
  // if (delay_synchro>0) delay_synchro -= dt_ms;
#endif
}


void ModemControl_Send(unsigned char * buf){
  test_set(1);
//	delay_measure_tx = system_getTime_ms();//0;	// start calculate tx time
  ModemControl_timeUpdate(); //update timer
  delay_measure_tx = 0;  // reset timer to caltulate time tx pack

#ifdef SI4463


  RFwrite(buf, PACKET_LEN);                                         // TX Data
#endif
#ifdef nRF24
  TX_Mode(buf);
#endif


	// delay_measure_tx = system_getTime_ms() - delay_measure_tx;	//calculate time for tx
	// if (delay_measure_tx > 100) delay_measure_tx=0;
	// delay_measure_tx++;
#if SYNCHRO_MODE
  ModemControl_timeUpdate(); //update timer
  if (delay_measure_tx <= 0)delay_measure_tx = 1;  // time tx minimum 1ms
  if (SYNCHRO_MODE) delay_to_wait_answer = delay_measure_tx+SYNCHRO_TWAIT;  //50; //Set timer between send the packets
  //delay_synchro = SYNCHRO_TIME;    //else delay_to_wait_answer = 10;	//delay_to_wait_answer = 10;  //50;

#endif
  test_set(0);
}

int ModemControl_Work(void)
{
	if (ModemControl_status < 0) ModemControl_init();
	ModemControl_timeUpdate();

  if (ModemControl_Read() != 0) return 1;

  if (ptr_SendSymbol>0) { //if to have symbols to send
    if (delay_after_send_symbol > 2) ModemControl_SendSymbol_Complete();
  }

#if 0
     if(0)  // UART Echo
     while(UartRxBuffer_rd!=UartRxBuffer_wr){
         UART1_SendData8((uint8_t)UartRxBuffer[UartRxBuffer_rd]);
         while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
         if (++UartRxBuffer_rd>=UART_BUF_SIZE) UartRxBuffer_rd = 0;
     }
#endif
     //if (SPI_Read_Reg(CD) != 0) {return;}

#if SYNCHRO_MODE
       if (delay_to_wait_answer > 0){return 0; }                            //The timer between the packets being sent. Otherwise, heap data was not accepted.
#endif
         unsigned char DataToSend = 0;

#define SEND_PACKETBLOCK 0

        	 if (DataEx(toSendCmd)){
#if SEND_PACKETBLOCK
		do{
			int future_RD = toSendCmd_RD;
			int middlePack = 0;
			DataArrayPP(future_RD);
			if (DataExAny(future, toSendCmd)) {middlePack = 1;}
			if (middlePack) {toSendCmdbuf[toSendCmd_RD][0] = PACKET_DATA_PACK;}
			else {
				toSendCmdbuf[toSendCmd_RD][0] = PACKET_DATA;
			}
#else
			toSendCmdbuf[toSendCmd_RD][0] = PACKET_DATA;
#endif
			//toSendCmdbuf[toSendCmd_RD][1] = (PACKET_DATALEN)&0x3F;
      //toSendCmdbuf[toSendCmd_RD][63] = pack_counter++;
			ModemControl_Send(toSendCmdbuf[toSendCmd_RD]);



			DataArrayPP(toSendCmd_RD);
#if SEND_PACKETBLOCK
		}while(DataEx(toSendCmd));
#endif
#ifdef nRF24
		RX_Mode();
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
#ifdef nRF24
         RX_Mode();
#endif
         }
#endif

       
         return 0;
}
