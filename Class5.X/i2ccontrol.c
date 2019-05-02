#include "i2ccontrol.h"
#include "i2c_master_noinit.h"

#define SLAVE_ADDR 0x20  //0b 010 0000

void initExpander() {

    i2c_master_setup();
    i2c_master_start();                     // make the start bit
    i2c_master_send(SLAVE_ADDR<<1|0);       // 0 indicate writing
    i2c_master_send(0x00);                  // write to IODIR
    i2c_master_send(0xF0);                  // GP7-GP4:read; GP3-GP0:write
    i2c_master_stop();                      // make the stop bit
}
void setExpander(char pin, char level) {
	//output 
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(SLAVE_ADDR<<1|0);       // 0 indicate writing
    i2c_master_send(0x0A);                  // write to OLAT
    i2c_master_send(level<<pin);            // the value to put in the register
    i2c_master_stop();                      // send a RESTART so we can begin reading 
}

char getExpander() {
    //input
    char input;
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(SLAVE_ADDR<<1|0);       // 0 indicate writing
    i2c_master_send(0x09);                  // write to GPIO
    i2c_master_restart();                   // send a RESTART so we can begin reading
    i2c_master_send((SLAVE_ADDR << 1) | 1); // send slave address, left shifted by 1,
                                            // and then a 1 in lsb, indicating read
    input = i2c_master_recv();              // receive a byte from the bus
    i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
    i2c_master_stop();                      // send STOP:  end transmission, give up bus
    return input;
}