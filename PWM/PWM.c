/**
 * PWM调光
 * 1. 由最低亮度变最高亮度3秒
 * 2. 等待3秒
 * 3. 由最高亮度变最低亮度3秒
 * 4. 等待3秒，再跳到步骤1
 */

#include <reg52.h>

// 1KHZ
#define MS 161

sbit LED = P1^0;
bit ADD = 1;
unsigned char i=0, k=0;

void delay(unsigned char t) {
	while(t--);
}

void Init_Timer0(void) {
	TMOD |= 0x01; // 使用模式1，16位定时器，使用“|”符号可以在使用多个定时器时不受影响
	// TH0 = 0x00; // 给定初值，这里使用定时器最大值从0开始计数一直到65535溢出
	// TL0 = 0x00;
	EA = 1; // 总中断打开
	ET0 = 1; // 定时器中断打开
	TR0 = 1; // 定时器开关打开
}
void Timer0_isr(void) interrupt 1 using 1 {
	TH0 = (65535-18000)/256; // 重新赋值12M晶振计算，指令周期1uS，
	TL0 = (65535-18000)%256; // 1mS方波半个周期500uS，即定时500次
	
	if(ADD == 1) {
		if(i < MS) {
			i++;
		} else if(++k == MS) {
			k = 0;
			ADD = 0;
		}
	} else {
		if(i > 0) {
			i--;
		} else if(++k == MS) {
			k = 0;
			ADD = 1;
		}
	}
}
void main(void) {
	Init_Timer0();
	while(1) {
		LED = 1;
		delay(i);
		LED = 0;
		delay(MS-i);
	}
}
