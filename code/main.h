
#pragma once
#ifndef __MAIN_H__
#define __MAIN_H__


#define STM8   //LINUX // STM32
#define SI4463//nRF24	//SI4463

void msleep(int ms);

#define usleep(x) msleep(x/1000)
void Printf (char * str);
char* itoa(int i, char b[]);
unsigned long GetTime_ms(void);
void test_set(int pos);

#ifdef STM8
#include "stm8s.h"
#include "spi.h"
#elif defined (STM32)
#include "stm32f1xx_hal.h"
#include "spi.h"
#elif defined (LINUX)
#include "spi.h"
#endif



#endif