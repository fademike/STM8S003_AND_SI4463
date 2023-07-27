/*
 * blinky.c
 *
 * Copyright 2014 Edward V. Emelianoff <eddy@sao.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
#include "ports_definition.h"
#include "interrupts.h"
#include "uart.h"
#include "main.h"

#include "ModemControl.h"

volatile unsigned long Global_time = 0L; // global time in ms	//TODO change to us!!!!
U16 paused_val = 1000; // interval between LED flashing

void my_msleep(int ms){
	unsigned long T = Global_time;
	while(((Global_time - T) < ms) && (T <= Global_time)){}
}

unsigned long GetTime_ms(void){
	return Global_time;
}

void Printf (char * str){
	int p=0;
	while(str[p] != '\0') UART_send_byte(str[p++]);
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}

void test_set(int pos){
	if (pos != 0)PORT(TEST_PORT, ODR) |= TEST_PIN;
	else PORT(TEST_PORT, ODR) &= ~TEST_PIN;
}

#include "spi.h"

int main() {
	unsigned long T = 0L;
	//U8 rb, waitfor = 0;
	//U8 *outbuf = "Hello, world";
	//U8 inbuf[13] = {0};
	//U8 inoutbuf[] = {'G','o','o','d','b','y','e',0};
	CFG_GCR |= 1; // disable SWIM
	// Configure clocking
	CLK_CKDIVR = 0; // F_HSI = 16MHz, f_CPU = 16MHz
// Timer 4 (8 bit) used as system tick timer
	// prescaler == 128 (2^7), Tfreq = 125kHz
	// period = 1ms, so ARR = 125
	TIM4_PSCR = 7;
	TIM4_ARR = 125;
	// interrupts: update
	TIM4_IER = TIM_IER_UIE;
	// auto-reload + interrupt on overflow + enable
	TIM4_CR1 = TIM_CR1_APRE | TIM_CR1_URS | TIM_CR1_CEN;

	// PC2 - PP output (on-board LED)
	PORT(LED_PORT, DDR)  |= LED_PIN;
	PORT(LED_PORT, CR1)  |= LED_PIN;

	// PC2 - PP output (on-board LED)
	PORT(TEST_PORT, DDR)  |= TEST_PIN;
	PORT(TEST_PORT, CR1)  |= TEST_PIN;


	uart_init();
	spi_init();
	spi_cs_init();
	spi_ce_init();


// while(1){
// 	char answer = spi_send_byte(0xAA);
// }


	// enable all interrupts
	enableInterrupts();

	int RF_init=-1;

	do{
		my_msleep(1000);
		// UART_send_byte('A');
		//Printf("hello\n\r");
		RF_init = ModemControl_init();

		if (RF_init < 0) Printf ("init err\n\r");	

	} while (RF_init<0);

	Printf("start...\n\r");	
	while(1){
		//my_msleep(100);
		//ModemControl_Read();
		ModemControl_Work();
		// //if (uart_tx_len <= 0){
		// 	char * b;
		// 	int len = ModemControl_GetPacket(b);
		// 	if (len>0) {
		// 		uart_tx_array = b;
		// 		uart_tx_ptr = 1;
		// 		uart_tx_len = len -1;
		// 		//uart_init();
		// 		UART1_CR2 |= UART_CR2_TIEN; // Allow RX/TX, generate ints on rx
		// 		UART_send_byte(b[0]);
				
		// 		// int p=0;
		// 		// while(p<len) UART_send_byte(b[p++]);
		// 	}
		// //}

		// if (uart_tx_len <= 0){
		// 	int len = 0;
		// 	char * b = ModemControl_GetPacketPtr(&len);
		// 	if (len>0) {
		// 		uart_tx_array = b;
		// 		uart_tx_ptr = 1;
		// 		uart_tx_len = len -1;
		// 		//uart_init();
		// 		UART1_CR2 |= UART_CR2_TIEN; // Allow RX/TX, generate ints on rx
		// 		UART_send_byte(b[0]);
				
		// 		// int p=0;
		// 		// while(p<len) UART_send_byte(b[p++]);
		// 	}
		// }
		// else {UART1_CR2 |= UART_CR2_TIEN;}

		
		char bb[64];
		while (ModemControl_GetByte(bb)==1)UART_send_byte(bb[0]);

		// if (ModemControl_GetPacket(b) == 1) {Printf("Rx: ");
		// // while (ret == 1) {
		// 	char s[5];
		// 	Printf(itoa((int)b[0], s));
		// 	Printf(", ");
		// 	Printf(itoa((int)b[1], s));
		// 	Printf(", ");
		// 	Printf(itoa((int)b[2], s));
		// 	Printf(", ");
		// 	Printf(itoa((int)b[3], s));
		// 	Printf(", ");
		// 	Printf(itoa((int)b[4], s));
		// 	Printf(", ");
		// 	Printf(itoa((int)b[5], s));
		// 	Printf("\n\r");
		// }


		// if(((Global_time - T) > 10) || (T > Global_time)){
		// 	static char ss=0;
		// 	T = Global_time;
		// 	//ModemControl_SendPacket(" ");
		// 	ModemControl_SendSymbol(ss++);
		// }

//ModemControl_SendSymbol('A');

		//Printf("Loop\n\r");
// enableInterrupts();
// 	if(UART1_SR & UART_SR_RXNE){ // data received
// 		unsigned char rb = UART1_DR; // read received byte & clear RXNE flag
// 		UART_send_byte(rb);
// 	// 	ModemControl_SendSymbol(rb);
// 	}
	}

	// // Loop
	// do{
	// 	if((Global_time - T > paused_val) || (T > Global_time)){
	// 		T = Global_time;
	// 		PORT(LED_PORT, ODR) ^= LED_PIN; // blink on-board LED
	// 	}
	// 	if(waitfor && spi_buf_sent()){
	// 		if(waitfor == 1)
	// 			uart_write(inbuf);
	// 		else
	// 			uart_write(inoutbuf);
	// 		waitfor = 0;
	// 	}
	// 	if(UART_read_byte(&rb)){ // buffer isn't empty
	// 		switch(rb){
	// 			case 'h': // help
	// 			case 'H':
	// 				UART_send_byte(rb);
	// 				uart_write("\nPROTO:\n+/-\tLED period\n"
	// 				);
	// 			break;
	// 			case '+':
	// 				UART_send_byte(rb);
	// 				paused_val += 100;
	// 				if(paused_val > 10000)
	// 					paused_val = 500; // but not more than 10s
	// 			break;
	// 			case '-':
	// 				UART_send_byte(rb);
	// 				paused_val -= 100;
	// 				if(paused_val < 500)  // but not less than 0.5s
	// 					paused_val = 500;
	// 			break;
	// 			case 's':
	// 				spi_init();
	// 			break;
	// 			case 'b':
	// 				printUHEX(SPI_SR);
	// 			break;
	// 			case '1':
	// 				spi_send_buffer(outbuf, 12, inbuf);
	// 				waitfor = 1;
	// 			break;
	// 			case '2':
	// 				spi_send_buffer(inoutbuf, 7, inoutbuf);
	// 				waitfor = 2;
	// 			break;
	// 			default:
	// 				UART_send_byte(spi_send_byte(rb)); // send received byte to SPI & write ans to UART
	// 		}
	// 	}
	// }while(1);
}






