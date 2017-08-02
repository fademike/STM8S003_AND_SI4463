# STM8S003_AND_SI4463
Simple Transceiver on SI4463 (HC-12)

![Image alt](https://github.com/fademike/STM8S003_AND_SI4463/blob/master/HC-12.jpg)

Such a thing can be bought on the ebay. Soldering STM8, because can not reprogram it.

In repository:
- Project IAR for STM8S. Program for simple tx-rx data by uart. If change flag SYNCHRO_MODE to 1 (Part not debugged), the program will synchronously receive and transmit data by uart. When the (by UART) appears in one data transmitter, it will begin to synchronize. This will protect against collisions. Synchronization time (500ms) can be changed.
- Project Wireless Development Suite (WDS) - Settings SI4463.
- PDF by STM8CubeMX

