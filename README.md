# STM8S003_AND_SI4463
Simple Transceiver on SI4463 (HC-12)

![Image alt](https://github.com/fademike/STM8S003_AND_SI4463/blob/master/HC-12.jpg)

Such a thing can be bought on the ebay. Soldering STM8, because can not reprogram it. Will not work with factory firmware

In repository:
- Project SDCC for STM8S. Program for simple tx-rx data by uart. 
- in the last commit, IAR the project
- STM8CubeMX Project + PDF

How to work sync mode:
![Image alt](https://github.com/fademike/STM8S003_AND_SI4463/blob/master/synchroMode.png)

If change flag SYNCHRO_MODE to 1 (in ModemControl.h), the program will synchronously receive and transmit data by uart. When the (by UART) appears in one data transmitter, it will begin to synchronize. This will protect against collisions. Synchronization time (500ms) can be changed.

In simple mode (SYNCHRO_MODE = 0), the transceiver is always in receive mode. When data appears by UART, it transmits (by RF) it and goes back to receive mode.


