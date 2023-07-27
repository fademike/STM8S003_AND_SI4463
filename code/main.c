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

volatile unsigned long Global_time = 0L; // global time in ms

void msleep(int ms){
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

	// enable all interrupts
	enableInterrupts();

	int RF_init=-1;

	do{
		msleep(1000);
		RF_init = ModemControl_init();
		if (RF_init < 0) Printf ("init err\n\r");	

	} while (RF_init<0);

	//Printf("start...\n\r");	
	while(1){
		ModemControl_Loop();
		
		char bb[64];
		while (ModemControl_GetByte(bb)==1)UART_send_byte(bb[0]);
	}
}






