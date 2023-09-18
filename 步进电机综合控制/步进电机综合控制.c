/*-----------------------------------------------
  ���ƣ��������
  ��̳��www.doflye.net
  ��д��shifang
  ���ڣ�2009.5
  �޸ģ���
  ���ݣ�
 ------------------------------------------------*/

#include <reg52.h>

#define KeyPort P3 // ��������IO
#define DataPort P0 // 8λ���������IO
sbit LATCH1 = P2 ^ 2; // 8λ����� ������
sbit LATCH2 = P2 ^ 3; //           λ����

// �������IO
sbit A1 = P1 ^ 0;
sbit B1 = P1 ^ 1;
sbit C1 = P1 ^ 2;
sbit D1 = P1 ^ 3;

sbit TC = P1 ^ 4; // ��ʱ������
sbit MC = P1 ^ 5; // ����������

const unsigned char code dofly_DuanMa[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; // ��ʾ����ֵ0~9
const unsigned char code dofly_WeiMa[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f}; //�ֱ��Ӧ��Ӧ������ܵ���,��λ��
unsigned char TempData[8]; //�洢��ʾֵ��ȫ�ֱ���

#define MaxSpeed 80
unsigned char Speed = MaxSpeed / 2;
unsigned short TimerCycle;
volatile unsigned short cycles = 0;
volatile unsigned int milliseconds = 0;
bit StopFlag = 0, reverseFlag = 0;

void Display(unsigned char FirstBit, unsigned char Num);
void Init_Timer0(void);
void Init_Timer1(void);
unsigned char KeyScan(void);

/*------------------------------------------------
                    ������
------------------------------------------------*/
void CalcSpeed(void)
{
    TimerCycle = (65536 - 1000 - (MaxSpeed - Speed) * 50);
}
main(void)
{
    unsigned char num, i;
    unsigned short n;
    unsigned int msec = 0;
    
    A1 = B1 = C1 = D1 = 0;
    CalcSpeed();
    Init_Timer0();
    Init_Timer1();
    while (1) { //����
        MC = 1;
        num = KeyScan();  //ѭ�����ð���ɨ��
        if (num == 1) { //��һ������,�ٶȵȼ�����
            if (Speed < MaxSpeed) {
                Speed++;
                CalcSpeed();
            }
        } else if (num == 2) { //�ڶ����������ٶȵȼ���С
            if (Speed) {
                Speed--;
                CalcSpeed();
            }
        } else if (num == 3) { //���������ֹͣ
            if(StopFlag) {
                StopFlag = 0;
            } else {
                A1 = B1 = C1 = D1 = 0;
                StopFlag = 1;
            }
        } else if (num == 4) { //�����ת����ת
            reverseFlag = !reverseFlag;
        }
        //�ֽ���ʾ��Ϣ����Ҫ��ʾ68����68/10=6  68%10=8
        TempData[0] = dofly_DuanMa[Speed / 10];
        TempData[1] = dofly_DuanMa[Speed % 10];
        
        n = cycles;
        for(i = 7; i >= 3; i --) {
          TempData[i] = dofly_DuanMa[n % 10];
          n /= 10;
        }
        
        Display(0, 8);
        
        MC = 0;
        
        // ��ʱ1����
        while(msec == milliseconds);
        msec = milliseconds;
    }
}

/*------------------------------------------------
 ��ʾ���������ڶ�̬ɨ�������
 ������� FirstBit ��ʾ��Ҫ��ʾ�ĵ�һλ���縳ֵ2��ʾ
 �ӵ���������ܿ�ʼ��ʾ
 ������0��ʾ�ӵ�һ����ʾ��
 Num��ʾ��Ҫ��ʾ��λ��������Ҫ��ʾ99��λ��ֵ���ֵ����2
------------------------------------------------*/
void Display(unsigned char FirstBit, unsigned char Num)
{
    static unsigned char i = 0;

    DataPort = 0; //������ݣ���ֹ�н�����Ӱ
    LATCH1 = 1;   //������
    LATCH1 = 0;

    DataPort = dofly_WeiMa[i + FirstBit]; //ȡλ��
    LATCH2 = 1;   //λ����
    LATCH2 = 0;

    DataPort = TempData[i]; //ȡ��ʾ���ݣ�����
    LATCH1 = 1;   //������
    LATCH1 = 0;

    i++;
    if (i == Num)
        i = 0;
}
/*------------------------------------------------
                    ��ʱ����ʼ���ӳ���
------------------------------------------------*/
void Init_Timer0(void)
{
    TMOD |= 0x01;    //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��
    TH0 = TimerCycle / 256;   //���¸�ֵ 1ms
    TL0 = TimerCycle % 256;
    EA  = 1;          //���жϴ�
    ET0 = 1;         //��ʱ���жϴ�
    TR0 = 1;         //��ʱ�����ش�
    PT0 = 1;         //���ȼ���
}
void Init_Timer1(void)
{
    TMOD |= 0x10;    //ʹ��ģʽ1��16λ��ʱ����ʹ��"|"���ſ�����ʹ�ö����ʱ��ʱ����Ӱ��
    TH1 = (65536 - 1000) / 256;   //���¸�ֵ 1ms
    TL1 = (65536 - 1000) % 256;
    EA  = 1;          //���жϴ�
    ET1 = 1;         //��ʱ���жϴ�
    TR1 = 1;         //��ʱ�����ش�
    PT1 = 1;         //���ȼ���
}
/*------------------------------------------------
                 ��ʱ���ж��ӳ���
------------------------------------------------*/
void Timer0_isr(void) interrupt 1
{
    static unsigned char times = 0, i = 0;
    static unsigned short n = 0;
    TH0 = TimerCycle / 256;   //���¸�ֵ 1ms
    TL0 = TimerCycle % 256;
    TC = 1;

    if(StopFlag) {
        times = 0;
        if(reverseFlag) i = 0;
        else i = 7;
        n = 0;
        cycles = 0;
    } else {
        switch (i) {
           case 8: // ��ת
                i = 0;
            case 0:
                A1 = 1;
                B1 = 0;
                C1 = 0;
                D1 = 0;
                break;
            case 1:
                A1 = 1;
                B1 = 1;
                C1 = 0;
                D1 = 0;
                break;
            case 2:
                A1 = 0;
                B1 = 1;
                C1 = 0;
                D1 = 0;
                break;
            case 3:
                A1 = 0;
                B1 = 1;
                C1 = 1;
                D1 = 0;
                break;
            case 4:
                A1 = 0;
                B1 = 0;
                C1 = 1;
                D1 = 0;
                break;
            case 5:
                A1 = 0;
                B1 = 0;
                C1 = 1;
                D1 = 1;
                break;
            case 6:
                A1 = 0;
                B1 = 0;
                C1 = 0;
                D1 = 1;
                break;
            default: // ��ת
                i = 7;
            case 7:
                A1 = 1;
                B1 = 0;
                C1 = 0;
                D1 = 1;
                break;
        }
        if(reverseFlag) i++;
        else i--;
        
        if(++n == 64 * 16) {
          n = 0;
          cycles ++;
        }
    }
    TC = 0;
}
void Timer1_isr(void) interrupt 3
{
    TH1 = (65536 - 1000) / 256;   //���¸�ֵ 1ms
    TL1 = (65536 - 1000) % 256;
    
    milliseconds ++;
}

/*------------------------------------------------
����ɨ�躯��������ɨ���ֵ
------------------------------------------------*/
unsigned char KeyScan(void)
{
    static unsigned char KEY = 0xff;
    static unsigned int msec = 0, msec2 = 0;
    
    unsigned char key = KeyPort, ret = 0;
    unsigned int val2;
    if (key != KEY) {
        if(KEY != 0xff) { // ����50��������
            val2 = milliseconds - msec;
            
            if(val2 > 50 && val2 < 500) {
                switch (KEY) {
                    case 0xfe:
                        ret = 1;
                        break;
                    case 0xfd:
                        ret = 2;
                        break;
                    case 0xfb:
                        ret = 3;
                        break;
                    case 0xf7:
                        ret = 4;
                        break;
                    case 0xef:
                        ret = 5;
                        break;
                    case 0xdf:
                        ret = 6;
                        break;
                    case 0xbf:
                        ret = 7;
                        break;
                    case 0x7f:
                        ret = 8;
                        break;
                    default:
                        break;
                }
            }
        }
        KEY = key;
        msec = milliseconds;
        msec2 = 0;
    } else if(key != 0xff && milliseconds - msec > 500) {
        val2 = (milliseconds - msec - 500) / 100;
        if(val2 != msec2) {
            msec2 = val2;
            
            switch (key) {
                case 0xfe:
                    ret = 1;
                    break;
                case 0xfd:
                    ret = 2;
                    break;
                case 0xfb:
                    ret = 3;
                    break;
                case 0xf7:
                    ret = 4;
                    break;
                case 0xef:
                    ret = 5;
                    break;
                case 0xdf:
                    ret = 6;
                    break;
                case 0xbf:
                    ret = 7;
                    break;
                case 0x7f:
                    ret = 8;
                    break;
                default:
                    break;
            }
        }
    }
    return ret;
}
