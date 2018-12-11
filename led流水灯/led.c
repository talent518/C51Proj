#include <reg52.h>

void delay();

main() {
	char i;
	unsigned char l, r;
	while(1) {
		// 单个点亮，从左到右
		for(i=0;i<8;i++) {
			P1 = ~(1<<i);
			delay();
		}
		// 单个点亮，从右到左
		for(i=7;i>=0;i--) {
			P1 = ~(1<<i);
			delay();
		}
		// 连续点亮，从左到右
		for(i=1;i<8;i++) {
			P1 = P1<<1;
			delay();
		}
		// 连续点亮，从右到左
		for(i=7;i>=0;i--) {
			P1 = P1|1<<i;
			delay();
		}
		// 从中间到两边连续点亮
		l=0xf;
		r=0xf0;
		for(i=0;i<8;i++) {
			if(i%2) l>>=1;
			else r<<=1;
			P1 = l|r;
			delay();
		}
		// 从两边到中间连续点亮
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
		unsigned int i; // 2字节无符号型
		for(i=0;i<20000;i++);
}
