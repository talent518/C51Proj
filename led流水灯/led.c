#include <reg52.h>

void delay();

main() {
	char i;
	unsigned char l, r;
	while(1) {
		// ����������������
		for(i=0;i<8;i++) {
			P1 = ~(1<<i);
			delay();
		}
		// �������������ҵ���
		for(i=7;i>=0;i--) {
			P1 = ~(1<<i);
			delay();
		}
		// ����������������
		for(i=1;i<8;i++) {
			P1 = P1<<1;
			delay();
		}
		// �������������ҵ���
		for(i=7;i>=0;i--) {
			P1 = P1|1<<i;
			delay();
		}
		// ���м䵽������������
		l=0xf;
		r=0xf0;
		for(i=0;i<8;i++) {
			if(i%2) l>>=1;
			else r<<=1;
			P1 = l|r;
			delay();
		}
		// �����ߵ��м���������
		l=0xf;
		r=0xf0;
		for(i=0;i<8;i++) {
			if(i%2) l = (l<<1) & 0xf;
			else r = (r>>1) & 0xf0;
			P1 = l|r;
			delay();
		}
	}
}

void delay() {
		unsigned int i; // 2�ֽ��޷�����
		for(i=0;i<20000;i++);
}
