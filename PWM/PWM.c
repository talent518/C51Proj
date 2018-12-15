/**
 * PWM����
 * 1. ��������ȱ��������3��
 * 2. �ȴ�3��
 * 3. ��������ȱ��������3��
 * 4. �ȴ�3�룬����������1
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
	TMOD |= 0x01; // ʹ��ģʽ1��16λ��ʱ����ʹ�á�|�����ſ�����ʹ�ö����ʱ��ʱ����Ӱ��
	// TH0 = 0x00; // ������ֵ������ʹ�ö�ʱ�����ֵ��0��ʼ����һֱ��65535���
	// TL0 = 0x00;
	EA = 1; // ���жϴ�
	ET0 = 1; // ��ʱ���жϴ�
	TR0 = 1; // ��ʱ�����ش�
}
void Timer0_isr(void) interrupt 1 using 1 {
	TH0 = (65535-18000)/256; // ���¸�ֵ12M������㣬ָ������1uS��
	TL0 = (65535-18000)%256; // 1mS�����������500uS������ʱ500��
	
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
