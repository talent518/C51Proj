#include <reg52.h>

sbit LED = P1^0; // P10
sbit LED1 = P1^1; // P11

void delay();

main() {
	while(1) {
		LED=0;
		LED1=1;
		delay();
		LED=1;
		LED1=0;
		delay();
	}
}

void delay() {
		unsigned int i; // 2×Ö½ÚÎÞ·ûºÅÐÍ
		for(i=0;i<60000;i++);
}