#include <reg52.h>

void Init_Timer0(void);
void init_led_data();

main() {
	init_led_data();
	Init_Timer0();
	while(1);
}

// LED灯数据
#define ledN 48
char ledi = 0, ledii = 0;
unsigned char leds[ledN];

void init_led_data() {
	char i, j=0;
	unsigned char l, r, p;
	
	// 单个点亮，从左到右
	for(i=0;i<8;i++) {
		leds[j++] = ~(1<<i);
	}
	// 单个点亮，从右到左
	for(i=7;i>=0;i--) {
		leds[j++] = ~(1<<i);
	}
	// 连续点亮，从左到右
	p = 0xff;
	for(i=1;i<8;i++) {
		p = p<<1;
		leds[j++] = p;
	}
	// 连续点亮，从右到左
	for(i=7;i>=0;i--) {
		p = p|1<<i;
		leds[j++] = p;
	}
	// 从中间到两边连续点亮
	l=0xf;
	r=0xf0;
	for(i=0;i<8;i++) {
		if(i%2) l>>=1;
		else r<<=1;
		leds[j++] = l|r;
	}
	// 从两边到中间连续点亮
	l=0xf;
	r=0xf0;
	for(i=0;i<8;i++) {
		if(i%2) l = (l<<1) & 0xf;
		else r = (r>>1) & 0xf0;
		leds[j++] = l|r;
	}
}

// 8位数码管数据
#define N 10
unsigned char nums[N] = {
	0x3f, // 0
	0x06, // 1
	0x5b, // 2
	0x4f, // 3
	0x66, // 4
	0x6d, // 5
	0x7d, // 6
	0x07, // 7
	0x7f, // 8
	0x6f // 9
};
unsigned char cathodeNums[N] = {
	0xc0, // 0
	0xf9, // 1
	0xa4, // 2
	0xb0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xf8, // 7
	0x80, // 8
	0x90 // 9
};
unsigned char addrs[8] = {
	0x7f,
	0xbf,
	0xdf,
	0xef,
	0xf7,
	0xfb,
	0xfd,
	0xfe
};

sbit LATCH1 = P2^2; // 段锁存
sbit LATCH2 = P2^3; // 位锁存
unsigned long LED8n = 0, n;
unsigned char LED8s[8] = {0,0,0,0,0,0,0,0}, LED8in = 1, LED8i=0;

void Init_Timer0(void) {
	TMOD |= 0x01; // 使用模式1，16位定时器，使用“|”符号可以在使用多个定时器时不受影响
	// TH0 = 0x00; // 给定初值，这里使用定时器最大值从0开始计数一直到65535溢出
	// TL0 = 0x00;
	EA = 1; // 总中断打开
	ET0 = 1; // 定时器中断打开
	TR0 = 1; // 定时器开关打开
}

void Timer0_isr(void) interrupt 1 using 1 {
	TH0 = (65535-500)/256; // 重新赋值12M晶振计算，指令周期1uS，
	TL0 = (65535-500)%256; // 1mS方波半个周期500uS，即定时500次
												 // 溢出然后输出端取反
	if(++ledii == 50) {
		ledii = 0;
		P1 = leds[ledi++];
		if(ledi == ledN) ledi = 0;
		
		if(ledi == 0) {
			n = LED8n;
			LED8in = 0;
			do {
				LED8s[LED8in] = n%10;
				n /= 10;
				LED8in++;
			} while(n);
			LED8n++;
			P3 = cathodeNums[LED8s[0]];
		}
	}
	
	P0 = addrs[LED8i];
	LATCH2=1;
	LATCH2=0;
	P0 = nums[LED8s[LED8i]];
	LATCH1=1;
	LATCH1=0;
	if(++LED8i == LED8in) {
		LED8i = 0;
	}
	P0 = 0xff;
}
