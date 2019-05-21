#ifndef touchscreen_H__
#define touchscreen_H__

void XPT2046_read(unsigned short *x, unsigned short *y, unsigned int *z);
unsigned short xPixel(unsigned short x);
unsigned short yPixel(unsigned short y);
void touchinit();
signed int buttonsPush(unsigned short xPixel, unsigned short yPixel);

#endif