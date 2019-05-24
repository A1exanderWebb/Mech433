#ifndef IMU_H__
#define IMU_H__
// Header file for imu.c
// IMU LSM6DS33 initialization & helper function

#define WHO_AM_I 0x0F
#define SLAVE_ADDR 0b1101010 //SDO/SAO pin unconnect, so LSbit is 0 (note: 7 bits total)
#define CTRL1_XL 0x10
#define CTRL2_G  0x11
#define CTRL3_C  0x12
#define CTRL1_XL_config 0x82
#define CTRL2_G_config  0x88
#define CTRL3_C_config  0x4

//multiple read
#define OUT_TEMP_L 0x20


void imu_init(void);
unsigned short whoAmI(void);
void I2C_read_multiple(unsigned char address, unsigned char reg, unsigned char *data, int length);

#endif