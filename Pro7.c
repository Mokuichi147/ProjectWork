#include <XC.h>
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
void turn (int L_ON, int R_ON, int TOTAL)
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

void turn_left  (int ON_Time, int TOTAL) { turn(      0, ON_Time, TOTAL); }
void turn_right (int ON_Time, int TOTAL) { turn(ON_Time,       0, TOTAL); }
void forward    (int ON_Time, int TOTAL) { turn(ON_Time, ON_Time, TOTAL); }


void main (void)
{
    TRISA = 0x1F;
    TRISB = 0x00;
    
    PORTB = 0x00;

    int Start = false;
    int Goal  = false;
    
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
                forward(7, 10);
                break;
            
            // 左側にライン -> 左に曲がる
            case 0b1110:
            case 0b1100:
            case 0b1000:
                M_L = M_LB = true;
                __delay_us(20);
                M_L = M_LB = false;
                turn_left(8, 10);
                Last = ZL;
                break;

            // 右側にライン -> 右に曲がる
            case 0b0111:
            case 0b0011:
            case 0b0001:
                M_R = M_RB = true;
                __delay_us(20);
                M_R = M_RB = false;
                turn_right(8, 10);
                Last = ZR;
                break;

            // 前回の結果により変える
            case 0b0100:
                switch (Last)
                {
                    // 左側にあるラインに近づいている ( |\ ) -> 少し右に曲がる
                    case ZL:
                        turn_right(8, 10);
                        break;

                    // 右側にあるラインに近づいている ( /| ) -> 少し左に曲がる
                    case R:
                    case ZR:
                        turn_left(8, 10);
                        break;

                    // スタートの可能性がある -> 直進
                    default:
                        forward(7, 10);
                        Last = L;
                        break;
                }
                break;
            
            // 前回の結果により変える
            case 0b0010:
                switch (Last)
                {
                    // 左側にあるラインに近づいている ( |\ ) -> 少し右に曲がる
                    case ZL:
                    case L:
                        turn_right(8, 10);
                        break;

                    // 右側にあるラインに近づいている ( /| ) -> 少し左に曲がる
                    case ZR:
                        turn_left(8, 10);
                        break;
                    
                    // スタートの可能性がある -> 直進
                    default:
                        forward(7, 10);
                        Last = R;
                        break;
                }
                break;
            
            // 前回の結果により変える
            case 0b0000:
                switch (Last)
                {
                    // 左側にライン -> 左に曲がる
                    case ZL:
                    // 左側にあるラインから離れている ( |/ ) -> 少し左に曲がる
                    case L:
                        turn_left(8, 10);
                        break;

                    // 右側にライン -> 右に曲がる
                    case ZR:
                    // 右側にあるラインから離れている ( \| ) -> 少し右に曲がる
                    case R:
                        turn_right(8, 10);
                        break;

                    // スタートの可能性がある -> 直進
                    default:
                        forward(7, 10);
                        break;
                }
                break;
            
            // 前回の結果により変える
            case 0b1111:
                switch (Last)
                {
                    // スタート -> 直進
                    case -1:
                        forward(7, 10);
                        break;
                    
                    // ゴール -> ループ終了
                    default:
                        //Goal = true;
                        break;
                }
                break;

            // 危険な値なので慣性で移動
            default:
                __delay_us(50);
                break;
        }
        one_ago = newest;
    }
}
