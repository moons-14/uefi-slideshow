#ifndef _COMMON_H
#define _COMMON_H

#define NULL	(void *)0
#define TRUE	1
#define FALSE	0
#define SC_OFS	0x1680
#define SC_ESC	(SC_OFS + 0x0017)

void putc(unsigned short c);
void puts(unsigned short *s);
void puth(unsigned long long val, unsigned char num_digits);
unsigned short getc(void);
unsigned int gets(unsigned short *buf, unsigned int buf_size);
int strcmp(unsigned short *s1, unsigned short *s2);
void strncpy(unsigned short *dst, unsigned short *src, unsigned long long n);
void assert(unsigned long long status, unsigned short *message);

#endif