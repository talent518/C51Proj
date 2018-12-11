#include <reg52.h>

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
unsigned char addrs[8] = {
	0xfe,
	0xfd,
	0xfb,
	0xf7,
	0xef,
	0xdf,
	0xbf,
	0x7f
};

sbit LATCH1 = P2^2; // ¶ÎËø´æ
sbit LATCH2 = P2^3; // Î»Ëø´æ

void delay(unsigned d);

main() {
	unsigned char i, j, ns[5];
	unsigned long n, nn;
	while(1) {
		for(n=0; n<100000000; n++) {
			i=0;
			nn = n;
			do {
				ns[i] = nn%10;
				nn /= 10;
				i++;
			} while(nn);
			nn = i;
			for(j=0; j<10/nn; j++) {
				for(i=0; i<nn; i++) {
					P0 = addrs[7-i];
					LATCH2=1;
					LATCH2=0;
					P0 = nums[ns[i]];
					LATCH1=1;
					LATCH1=0;
					delay(250);
				}
			}
		}
	}
}

void delay(unsigned d) {
		unsigned int i; // 2×Ö½ÚÎÞ·ûºÅÐÍ
		for(i=0;i<d;i++);
}
