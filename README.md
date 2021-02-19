# STM32 Status Monitor
## For Blue Pill (STM32F103C8T6) microcontrollers

Simple application to display incoming protobuf status messages over UART1 onto a 128x64 monochrome SSD1306 OLED display (via I2C1).

Uses DMA to process UART1, and polling to display. Printf debugging is enabled for UART2.

See accompanying project [linux-status-monitor](https://github.com/marcosatti/linux-status-monitor).

Developed using STM32CubeIDE.

### Architecture
CMSIS-RTOS v2 (FreeRTOS implementation) is used to drive this application, with the following threads being used:
- Status message parser thread, which decodes the incoming packets (see below) into status messages.
- Display thread, which formats strings from the status message and presents them on the OLED display.

A global status message queue is used which is pushed into by the message parser thread, and pulled by the display thread.

Within the message parser thread, it assumes a packet is delivered into the DMA buffer when the idle line interrupt is triggered (uses a binary semaphore to signal this). If the incoming packet passes all the checks, the underlying message is pushed into the queue above, and the cycle repeats.

### Packet Format

For robustness, the data sent over UART1 is assumed to be as packets; packets follow the below format, and an idle line interrupt is used to determine when a new packet is incoming (ie: each break in transmission is considered a new packet).
In a home setup, this is probably not needed, however it was a good learning exercise for me on how engineers might design against a noisy/unreliable line.

Packet format:
|- 1 byte -|--------- 16 bit --------|- N bytes -|- 1 byte -|
_____________________________________________________________
|          |            |            |           |          |
|          |    LSB     |    MSB     |   COBS    |          |
|   0x00   |  CHECKSUM  |  CHECKSUM  |   DATA    |   0x00   |
|          |            |            |           |          |
_____________________________________________________________

Do not exceed a packet length of 1024 (by default), otherwise the application will deadlock.

### Checksum Format
The checksum is a simple addition of the data (AND 0xFFFF) before it is encoded into COBS format. It is 16 bits long, and sent in little-endian format.
For extra robustness, a CRC16/32 checksum could be used.

### COBS Data Format
[COBS](https://github.com/cmcqueen/cobs-c) has been used to encode the data. See there for more details about the format. COBS is used so that we can delimit the packet with `0x00` at the start and end, which the code checks for; this provides extra robustness.

### Protobuf Messages
The status message expected to be sent is done so as a protobuf message, using [nanopb](https://github.com/nanopb/nanopb) to decode it. Done as a learning exercise again.

### Display Driver
Uses the SSD1306 display driver found [here](https://github.com/afiskon/stm32-ssd1306).

### Third-Paty Components
This project uses a variety of third-party components - see source code for licence details.
