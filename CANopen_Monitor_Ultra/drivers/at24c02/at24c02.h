#ifndef __AT24C02_H
#define __AT24C02_H

void At24c02_init(void);
void ByteWrite(unsigned char addr,unsigned char data);
unsigned char ReadByte(unsigned char addr);
void test(void);
#endif
