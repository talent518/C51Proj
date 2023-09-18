/*-----------------------------------------------
  名称：步进电机
  论坛：www.doflye.net
  编写：shifang
  日期：2009.5
  修改：无
  内容：
 ------------------------------------------------*/

#include <reg52.h>

#define KeyPort P3 // 独立按键IO
#define DataPort P0 // 8位数码管数据IO
sbit LATCH1 = P2 ^ 2; // 8位数码管 段锁存
sbit LATCH2 = P2 ^ 3; //           位锁存

// 步进电机IO
sbit A1 = P1 ^ 0;
sbit B1 = P1 ^ 1;
sbit C1 = P1 ^ 2;
sbit D1 = P1 ^ 3;

sbit TC = P1 ^ 4; // 计时器周期
sbit MC = P1 ^ 5; // 主函数周期

const unsigned char code dofly_DuanMa[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f}; // 显示段码值0~9
const unsigned char code dofly_WeiMa[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f}; //分别对应相应的数码管点亮,即位码
unsigned char TempData[8]; //存储显示值的全局变量

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
                    主函数
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
    while (1) { //正向
        MC = 1;
        num = KeyScan();  //循环调用按键扫描
        if (num == 1) { //第一个按键,速度等级增加
            if (Speed < MaxSpeed) {
                Speed++;
                CalcSpeed();
            }
        } else if (num == 2) { //第二个按键，速度等级减小
            if (Speed) {
                Speed--;
                CalcSpeed();
            }
        } else if (num == 3) { //电机启动或停止
            if(StopFlag) {
                StopFlag = 0;
            } else {
                A1 = B1 = C1 = D1 = 0;
                StopFlag = 1;
            }
        } else if (num == 4) { //电机倒转或正转
            reverseFlag = !reverseFlag;
        }
        //分解显示信息，如要显示68，则68/10=6  68%10=8
        TempData[0] = dofly_DuanMa[Speed / 10];
        TempData[1] = dofly_DuanMa[Speed % 10];
        
        n = cycles;
        for(i = 7; i >= 3; i --) {
          TempData[i] = dofly_DuanMa[n % 10];
          n /= 10;
        }
        
        Display(0, 8);
        
        MC = 0;
        
        // 延时1毫秒
        while(msec == milliseconds);
        msec = milliseconds;
    }
}

/*------------------------------------------------
 显示函数，用于动态扫描数码管
 输入参数 FirstBit 表示需要显示的第一位，如赋值2表示
 从第三个数码管开始显示
 如输入0表示从第一个显示。
 Num表示需要显示的位数，如需要显示99两位数值则该值输入2
------------------------------------------------*/
void Display(unsigned char FirstBit, unsigned char Num)
{
    static unsigned char i = 0;

    DataPort = 0; //清空数据，防止有交替重影
    LATCH1 = 1;   //段锁存
    LATCH1 = 0;

    DataPort = dofly_WeiMa[i + FirstBit]; //取位码
    LATCH2 = 1;   //位锁存
    LATCH2 = 0;

    DataPort = TempData[i]; //取显示数据，段码
    LATCH1 = 1;   //段锁存
    LATCH1 = 0;

    i++;
    if (i == Num)
        i = 0;
}
/*------------------------------------------------
                    定时器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
    TMOD |= 0x01;    //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响
    TH0 = TimerCycle / 256;   //重新赋值 1ms
    TL0 = TimerCycle % 256;
    EA  = 1;          //总中断打开
    ET0 = 1;         //定时器中断打开
    TR0 = 1;         //定时器开关打开
    PT0 = 1;         //优先级打开
}
void Init_Timer1(void)
{
    TMOD |= 0x10;    //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响
    TH1 = (65536 - 1000) / 256;   //重新赋值 1ms
    TL1 = (65536 - 1000) % 256;
    EA  = 1;          //总中断打开
    ET1 = 1;         //定时器中断打开
    TR1 = 1;         //定时器开关打开
    PT1 = 1;         //优先级打开
}
/*------------------------------------------------
                 定时器中断子程序
------------------------------------------------*/
void Timer0_isr(void) interrupt 1
{
    static unsigned char times = 0, i = 0;
    static unsigned short n = 0;
    TH0 = TimerCycle / 256;   //重新赋值 1ms
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
           case 8: // 倒转
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
            default: // 正转
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
    TH1 = (65536 - 1000) / 256;   //重新赋值 1ms
    TL1 = (65536 - 1000) % 256;
    
    milliseconds ++;
}

/*------------------------------------------------
按键扫描函数，返回扫描键值
------------------------------------------------*/
unsigned char KeyScan(void)
{
    static unsigned char KEY = 0xff;
    static unsigned int msec = 0, msec2 = 0;
    
    unsigned char key = KeyPort, ret = 0;
    unsigned int val2;
    if (key != KEY) {
        if(KEY != 0xff) { // 按下50毫秒以上
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
