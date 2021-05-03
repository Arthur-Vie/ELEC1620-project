# Localisation using a low-cost 6 axis accelerometer

---

The purpose of this project is to assess the feasibility of using the data from a cheap MEMS accelerometer IC like the LSM9DS1 to track the position and orientation of an object in 3D space.

The code has been written for the ATmega328p microcontroller and the Adafruit LSM9DS1 accelrometer and gyroscope module (https://learn.adafruit.com/adafruit-lsm9ds1-accelerometer-plus-gyro-plus-magnetometer-9-dof-breakout).

## Dependencies building and flashing

### You will need the following:

- avr-gcc
- avr-libc
- avr-objcopy
- avrdude

### To compile, run:

    avr-gcc -Wall -Os -mmcu=atmega328p -o prog.o AV_SPI.cpp AV_USART.cpp string_handling.cpp phase_5.cpp

### To generate a hex file:

    avr-objcopy -O ihex prog.o prog.hex

### To flash to the microcontroller:

    avrdude -v -c avrisp -p ATMEGA328P -b 19200 -P /dev/ttyUSB3 -U flash:w:prog.hex
