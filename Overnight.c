#include <xc.h>
#include <pic16f84a.h>

#pragma config FOSC  = HS
#pragma config WDTE  = OFF
#pragma config PWRTE = ON

#define _XTAL_FREQ 10000000

/* わかりやすくするため */
#define true  1
#define false 0

#define ZR 0
#define R  1
#define L  2
#define ZL 3

// モーター		例: 右へ曲がる ->  M_R = false, M_L = true
#define M_R  PORTBbits.RB7
#define M_L  PORTBbits.RB4
#define M_RB PORTBbits.RB6
#define M_LB PORTBbits.RB5

// __delay_usの時間
#define time_us 1500

/* モーターを動かす為の関数 */
void PWM (int L_ON, int R_ON, int TOTAL)
{
    if (L_ON)   M_L = true;
    if (R_ON)   M_R = true;
    for (int count=0; count<TOTAL; count++)
    {
        if (count == L_ON)  M_L = false;
        if (count == R_ON)  M_R = false;
        __delay_us(time_us);
    }
}

void turn_left  (int ON_Time, int TOTAL) { PWM(      0, ON_Time, TOTAL); }
void turn_right (int ON_Time, int TOTAL) { PWM(ON_Time,       0, TOTAL); }
void forward    (int ON_Time, int TOTAL) { PWM(ON_Time, ON_Time, TOTAL); }

void TURN (int ZLorZR)
{
    int Loop = true;
    int Sensor;
    int Prev = 0;
    int Current;

    // 前回の値に基づき本体の中心にラインが来るようにする
    while (Loop)
    {
        Sensor = ~PORTA & 0b1111;
        Current = Sensor & Prev;
        switch (Current)
        {
            case 0b0100:
            case 0b0010:
            case 0b0110:
                Loop = false;
                break;
            
            default:
                if (ZLorZR == ZL)
					turn_left(10, 10);
                else
					turn_right(10, 10);
                break;
        }
        Prev = Sensor;
    }
}

void main (void)
{
    TRISA = 0x1F;
    TRISB = 0x00;
    PORTB = 0x00;

    // スタートボタンとゴールのフラグ
    int Start = false;
    int Goal = false;
    // 前回の結果を保持する
    int Last = -1;
    // 誤差を抑える為に使う
    int newest;
    int one_ago = 0;
    int now;
    
    /* スタートボタン */
    while (PORTAbits.RA4 || !Start)
        Start = PORTAbits.RA4;
    
    /* ライントレース */
    while (!Goal)
    {
        // 最新のセンサーの値を取得し、白を1にする (ラインがあると1)
        newest = ~PORTA & 0b1111;
        now = newest & one_ago;
        
        switch (now)
        {
            // 本体の真ん中にライン -> 直進
            case 0b0110:
                forward(10, 10);
                break;
            
            // 左側にライン -> 左に旋回しラインを中心に戻す
            case 0b1000:
            case 0b1100:
            case 0b1110:
                TURN(ZL);
                Last = ZL;
                break;
            
            // 右側にライン -> 右に旋回しラインを中心に戻す
            case 0b0001:
            case 0b0011:
            case 0b0111:
                TURN(ZR);
                Last = ZR;
                break;
            
            // 左側にライン -> 少し左に曲がる
            case 0b0100:
                PWM(7, 10, 10);
                Last = L;
                break;
            
            // 右側にライン -> 少し右に曲がる
            case 0b0010:
                PWM(10, 7, 10);
                Last = R;
                break;
            
            // 前回の値に基づき中心にラインが来るように調整
            case 0b0000:
                switch (Last)
                {
                    case L:
                        PWM(4, 10, 10);
                        break;
                    
                    case R:
                        PWM(10, 4, 10);
                        break;
                    
                    default:
                        forward(10, 10);
                        break;
                }
                break;
            
            // スタートとゴール、90度カーブの時
            case 0b1111:
                switch (Last)
                {
                    case -1:
                        forward(10, 10);
                        break;
                    
                    default:
                        //Goal = true;
                        break;
                }
            
            default:
                __delay_ms(1);
                break;
        }
        one_ago = newest;
    }
}