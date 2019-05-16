#include "imu.h"
#include "i2c_master_noint.h" 
#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro

void imu_init(void) {
    i2c_master_setup();
    i2c_master_start();                     // make the start bit
    i2c_master_send(SLAVE_ADDR<<1|0);       // 0 indicate writing
    i2c_master_send(CTRL1_XL);
    i2c_master_send(CTRL1_XL_config);
    i2c_master_stop();
    // write to CTRL1_XL register
    i2c_master_setup();
    i2c_master_start();
    i2c_master_send(SLAVE_ADDR<<1|0);      // 
    i2c_master_send(CTRL2_G);
    i2c_master_send(CTRL2_G_config);
    i2c_master_stop();// write to CTRL2_G register
    
    // 
    i2c_master_setup();
    i2c_master_start();
    i2c_master_send(CTRL3_C);               // write to CTRL3_C register
    i2c_master_send(CTRL3_C_config); 
    i2c_master_stop();                      // make the stop bit
}

unsigned short whoAmI(void) {
    //input
    unsigned short input;
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(SLAVE_ADDR << 1 | 0);   // 0 indicate writing
    i2c_master_send(WHO_AM_I);              // write to WHO AM I Register
    i2c_master_restart();                   // send a RESTART so we can begin reading
    i2c_master_send(SLAVE_ADDR << 1 | 1);   // send slave address, left shifted by 1,
                                            // and then a 1 in lsb, indicating read
    input = i2c_master_recv();              // receive a byte from the bus
    i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
    i2c_master_stop();                      // send STOP:  end transmission, give up bus
    return input;
}

void I2C_read_multiple(unsigned char address, unsigned char register2read, unsigned char *data, int length) { 
    //unsigned char data[length];
    unsigned short i;
    i2c_master_start();                     // Begin the start sequence
    i2c_master_send(address << 1);      // 0 indicate writing
    i2c_master_send(register2read);         // write to WHO AM I Register
    i2c_master_restart();                   // send a RESTART so we can begin reading
    i2c_master_send((address << 1) | 1);      // send slave address, left shifted by 1,
                                            // and then a 1 in lsb, indicating read
    for (i = 0; i < length-1; i++) { //14
        data[i] = i2c_master_recv();        // receive a byte from the bus
        //if (length == 13)
        i2c_master_ack(0);                  // send NACK (0):  master needs more bytes
    }
    data[length - 1] = i2c_master_recv();   // receive a byte from the bus
    i2c_master_ack(1);                      // send NACK (1):  master needs no more bytes
    i2c_master_stop();                      // send STOP:  end transmission, give up bus
}
