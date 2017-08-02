/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "main.h"

#include "radio_config_Si4463.h"
#include "si446x_defs.h"

#include "RfTransceiver.h"
#include "Si4463.h"

/* Private defines -----------------------------------------------------------*/


#define LED_GPIO_PORT  (GPIOB)
#define LED_GPIO_PINS  (GPIO_PIN_5 | GPIO_PIN_2 | GPIO_PIN_1 | GPIO_PIN_0)

#define PACKET_LEN 64

#define SYNCHRO_MODE 0


  #define NONE_P  GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5); Delay_ms(1); GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5)
  //#define NONE_P  Delay_ms(1)

typedef enum  {
	PACKET_NONE 				= 0,
	PACKET_DATA				= 1,
	PACKET_ASK				= 2,
	PACKET_ANSWER				= 3,
};

/* Private function prototypes -----------------------------------------------*/

volatile int time_delay=0;
//volatile int time_last_tx=0;
volatile int delay_to_wait_answer=0;
volatile int delay_measure_tx=0;
volatile int delay_synchro=0;
char NeedAnswer=0;
int delay_measure_tx_middle=0;


  char UartRxBuffer[UART_BUF_SIZE];
  volatile int UartRxBuffer_rd = 0;
  volatile int UartRxBuffer_wr = 0;
  
  

void Delay (uint16_t nCount);
void Printf(char *fmt);
int GetChar();
static void CLK_Config(void);
char * IntToStr(unsigned int num);
void Delay_ms(int i);
/* Private functions ---------------------------------------------------------*/

void main(void)
{
      unsigned char rbuff[PACKET_LEN],  wbuff[PACKET_LEN];
      int y=0;

      /* Initialize I/Os in Output Mode */
  GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);            // PIN SET
  //GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
  
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);    //NSEL
  GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);    //nIRQ
  GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);    //GRIO1
  GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_IN_PU_NO_IT);    //GPIO0
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);    //SDN
  
  GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
  GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);
  
  
  
  
    
  /*High speed internal clock prescaler: 1*/
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    
  UART1_DeInit();

  UART1_Init((uint32_t)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO,
              UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE);
  
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);   //UART1_IT_RXNE
    
    

    
    
      //TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 4095, 0);
  TIM1_TimeBaseInit(2, TIM1_COUNTERMODE_UP, 4095, 0);
  TIM1_CCPreloadControl(ENABLE);
  TIM1_ITConfig(TIM1_IT_COM, ENABLE);
  TIM1_Cmd(ENABLE);

  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
    
  enableInterrupts();
  
  
    SPI_DeInit();
  /* Initialize SPI in Master mode  */
  SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_8, SPI_MODE_MASTER, SPI_CLOCKPOLARITY_LOW,//SPI_CLOCKPOLARITY_LOW,
           SPI_CLOCKPHASE_1EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX, SPI_NSS_SOFT,(uint8_t)0x07);

  /* Enable the UART1*/
  UART1_Cmd(ENABLE);
  
	Delay(0xFFF);
  
	/* Enable the SPI*/
  SPI_Cmd(ENABLE);
  
  if(1){                                                                        // Test to Read Device Name
  unsigned char buffer[16];
  	SI446X_PART_INFO(buffer);
        
        if ((buffer[2] != 0x44) || (buffer[3]!= 0x63)) {
          Printf("ERROR: Device is:");
        Printf(IntToStr(buffer[2]));
        Printf(",");
        Printf(IntToStr(buffer[3]));
        Printf("\n\r");
        Delay_ms(1000); 
        return;}
  }
  Printf("Device is SI4463\n\r");
  
  // RESET SI4463
  GPIO_WriteHigh(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);                    // SDN Hight
  Delay_ms(10);
  GPIO_WriteLow(GPIOD, (GPIO_Pin_TypeDef)GPIO_PIN_4);                     // SDN Low
  Delay_ms(100);

  MYinit();               ////////////////////////////////////////////////////////////////////////////////////////////////////////init

  SI446X_FIFOINFO(0, 0, 1, 1);		// BUFFER CLEAR
  changeState(STATE_RX);

  Printf("Started\n\r");
        
    while(1){
      
     uint8_t xStatus = getStatus();
      
      if ((xStatus&(1<<4))!=0){	                                                //PACKET_RX
        
        //if ((xStatus&(1<<3))!=0){GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);Delay_ms(1);GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);changeState(STATE_RX); continue;}
        
              RFread(rbuff, PACKET_LEN);
#if SYNCHRO_MODE
              if (rbuff[0] == PACKET_NONE ){
                NONE_P;
                NeedAnswer = 0; delay_to_wait_answer = 0;
              }
              else if (rbuff[0] == PACKET_ASK ){
                NONE_P;
                NeedAnswer = PACKET_ANSWER; delay_to_wait_answer = 0;
              }
              else if (rbuff[0] == PACKET_ANSWER ){
                NONE_P;
                NeedAnswer = 0; delay_to_wait_answer = 0;
              }
              else 
#endif
                if (rbuff[0] == PACKET_DATA){

                NONE_P;
                    
                NeedAnswer = PACKET_ANSWER; delay_to_wait_answer = 0;
                char cnt = rbuff[1]&0x3F;
                char i=2;
                if (cnt >=1){
                  do{
                    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
                    UART1_SendData8((uint8_t)rbuff[i++]);
                  }while(--cnt>0);
                }
              }
              changeState(STATE_RX);
              continue;
      }
          
      if(0)                                                                     // UART Echo
      while(UartRxBuffer_rd!=UartRxBuffer_wr){
          UART1_SendData8((uint8_t)UartRxBuffer[UartRxBuffer_rd]);
          while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
          if (++UartRxBuffer_rd>=UART_BUF_SIZE) UartRxBuffer_rd = 0;
      }

      

      if (1){   //tx data

        if (delay_to_wait_answer == 0){                                         //The timer between the packets being sent. Otherwise, heap data was not accepted. 

          char DataToSend = 0;
          if(UartRxBuffer_rd!=UartRxBuffer_wr){
            //unsigned char int_status[9];
            //SI446X_INT_STATUS(int_status);
            //if (((int_status[5]&0x03)!=0x00) || ((int_status[6]&0x03)!=0x00)) continue;
            //if ((int_status[6]&0x03)!=0x00) continue;
          
            
            while((DataToSend<60) && (UartRxBuffer_rd!=UartRxBuffer_wr)) {wbuff[2+DataToSend++] = UartRxBuffer[UartRxBuffer_rd]; if (++UartRxBuffer_rd>=UART_BUF_SIZE) UartRxBuffer_rd = 0;}
            wbuff[1] = (DataToSend)&0x3F;
            wbuff[0] = PACKET_DATA;
            
            
            delay_measure_tx = 0;
            
            GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
            RFwrite(wbuff, PACKET_LEN);                                         // TX Data
            GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
            

            if (SYNCHRO_MODE) delay_to_wait_answer = delay_measure_tx*5;  //50; //Set timer between send the packets
            else delay_to_wait_answer = 10;  //50;
            delay_synchro = 500;
          }
#if SYNCHRO_MODE
        }

          if ((DataToSend == 0) && ((NeedAnswer>0) || (delay_synchro>0))){      //Maintain communication by sync pulses.
            
            Delay_ms(delay_measure_tx_middle);                                  //So as not to foul the broadcast.
            
            if (NeedAnswer) {wbuff[0] = NeedAnswer; NeedAnswer=0;}              // Pulse is Answer
            else if (delay_synchro>0) wbuff[0] = PACKET_ASK;                    // Pulse is Ask
            else wbuff[0] = PACKET_NONE;                                        // Pulse is None
            //wbuff[0] &=0x03;
            delay_measure_tx = 0;
            GPIO_WriteHigh(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
            RFwrite(wbuff, PACKET_LEN);                                         // TX Synchro Pulse
            GPIO_WriteLow(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5);
            
            delay_measure_tx_middle = delay_measure_tx;
            delay_to_wait_answer = delay_measure_tx*5;  //50;
          }
#endif
          
        }
      }
        //Delay_ms(1000);
    }


  
  /* Infinite loop */
  while (1)
  {
    
    
    
    
    
  }
  
}
  char s[10] = {0,0,0,0,0,0,0,0,0,0};

char * IntToStr(unsigned int num)
{
  char ss[10] = {0,0,0,0,0,0,0,0,0,0};

  int i=0;
  int cnt=0;
  s[0] = '0';
  s[1] = '\0';
  if (num == 0)return s;
  for (i=0;(i<9)&&(num>0);i++) 
  {
    char cTemp = num%10;
    ss[i] = cTemp;
    ss[i] += 0x30;
    num /=10;
  }
  for (; i>0; i--)
  {
    s[cnt++]=ss[i-1];
  }
  s[cnt++]='\0';
  
  return s;
}


void Delay_ms(int delay)
{
  //int i = delay;
  //while((i>0)){while(TIM1_GetCounter()!=0);while(TIM1_GetCounter()==0); i--;}
  time_delay=delay;
  while(time_delay!=0){Delay(100);};
}


void Printf(char *fmt)
{ return;
	while(*fmt) {
		//HAL_UART_Transmit(&huart2, (unsigned char *)p, 1, 500);
                UART1_SendData8((uint8_t)*fmt);
                // Loop until the end of transmission 
                while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
		fmt++;
	}
}

int GetChar(void)
{
  int c = 0;

  while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
    c = UART1_ReceiveData8();
  return (c);
}



void Delay(uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}



static void CLK_Config(void)
{
    CLK_DeInit();
  
  /* Clock divider to HSI/1 */
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  /* Output Fcpu on CLK_CCO pin */
  //CLK_CCOConfig(CLK_OUTPUT_MASTER);
    
    
}



#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
