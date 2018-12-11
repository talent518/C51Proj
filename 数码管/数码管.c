#include <reg52.h>

#define N 11
unsigned char nums[N] = {
	0xc0, // 0
	0xf9, // 1
	0xa4, // 2
	0xb0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xf8, // 7
	0x80, // 8
	0x90, // 9
	0x10
};

void delay();

main() {
	char i;
	while(1) {
		for(i=0; i<N; i++) {
			P1 = nums[i];
			delay();
		}
	}
}

void delay() {
		unsigned int i; // 2×Ö½ÚÎÞ·ûºÅÐÍ
		for(i=0;i<33000;i++);
}
