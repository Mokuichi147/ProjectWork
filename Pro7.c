#include <XC.h>
#include <pic16f84a.h>

#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = ON

#define _XTAL_FREQ 10000000

/* わかりやすくするため */
#define true  1
#define false 0

#define ZR 0
#define R  1
#define L  2
#define ZL 3

/* sensor */
#define S_Start PORTAbits.RA4

/* motor */
// turn right  ->  M_R = false, M_L = true
#define M_R PORTBbits.RB7
#define M_L PORTBbits.RB4

#define M_RB PORTBbits.RB6
#define M_LB PORTBbits.RB5

void turn_left (int N, int M)
{
	M_L = false;
	M_R = true;
	for (int i=0; i<N; i++)
		__delay_us(100);
	M_R = false;
	for (int i=0; i<M; i++)
		__delay_us(100);
}

void turn_right (int N, int M)
{
	M_L = true;
	M_R = false;
	for (int i=0; i<N; i++)
		__delay_us(100);
	M_L = false;
	for (int i=0; i<M; i++)
		__delay_us(100);
}

void forward (int N, int M)
{
	for (int i=0; i<N; i++)
		__delay_us(100);
	M_L = M_R = false;
	for (int i=0; i<M; i++)
		__delay_us(100);
	M_L = M_R = true;
}

void forward_turn (int N, int M, int T)
{
	M_L = M_R = false;
	for (int i=0; i<T; i++)
	{
		if (i==N)
			M_L = true;
		if (i==M)
			M_R = true;
		__delay_us(100);
	}
}

void main (void)
{
	TRISA = 0x1F;
	TRISB = 0x00;
	
	PORTB = 0x00;

	int Start = false;
	int Goal = false;
	
	int Last = -1;

	int newest;
	int one_ago = 0;
	int two_ago = 0;

	int now;

	/* スタートボタン */
	while (S_Start || !Start)
		Start = S_Start;

	/* ライントレース */
	while (!Goal)
	{
		// 最新のセンサーの値を取得する
		newest = ~PORTA & 0b1111;

		now = newest & one_ago;
		now |= newest & two_ago;
		now |= one_ago & two_ago;

		/* 1 -> line */
		switch (now)
		{
			case 0b0110:
				// 本体の真ん中にライン -> 直進
				forward(7, 3);
				break;
			
			case 0b1110:
			case 0b1100:
			case 0b1000:
				// 左側にライン -> 左に曲がる
				M_L = M_LB = true;
				__delay_us(2);
				M_L = M_LB = false;
				turn_left(6, 4);
				//forward_turn(9, 4, 10);
				Last = ZL;
				break;

			case 0b0111:
			case 0b0011:
			case 0b0001:
				// 右側にライン -> 右に曲がる
				M_R = M_RB = true;
				__delay_us(2);
				M_R = M_RB = false;
				turn_right(6, 4);
				//forward_turn(4, 9, 10);
				Last = ZR;
				break;

			case 0b0100:
				/* 前回の結果により変える */
				switch (Last)
				{
					case ZL:
						// 左側にあるラインに近づいている -> 少し右に曲がる
						forward_turn(5, 8, 10);
						break;

					case R:
					case ZR:
						// 右側にあるラインに近づいている -> 少し左に曲がる
						forward_turn(8, 5, 10);
						break;

					default:
						// スタートの可能性がある -> 直進
						forward(7, 3);
						Last = L;
						break;
				}
				break;
			
			case 0b0010:
				/* 前回の結果により変える */
				switch (Last)
				{
					case ZL:
					case L:
						// 左側にあるラインに近づいている -> 少し右に曲がる
						forward_turn(5, 8, 10);
						break;

					case ZR:
						// 右側にあるラインに近づいている -> 少し左に曲がる
						forward_turn(8, 5, 10);
						break;
					
					default:
						// スタートの可能性がある -> 直進
						forward(7, 3);
						Last = R;
						break;
				}
				break;
			
			case 0b0000:
				/* 前回の結果により変える */
				switch (Last)
				{
					case ZL:
						// 左側にライン -> 左に曲がる
						turn_left(6, 4);
						//forward_turn(9, 4, 10);
						break;

					case L:
						// 左側にあるラインから離れている -> 少し左に曲がる
						forward_turn(8, 5, 10);
						break;

					case R:
						// 右側にあるラインから離れている -> 少し右に曲がる
						forward_turn(5, 8, 10);
						break;

					case ZR:
						// 右側にライン -> 右に曲がる
						turn_right(6, 4);
						//forward_turn(4, 9, 10);
						break;

					default:
						// スタートの可能性がある -> 直進
						forward(7, 3);
						break;
				}
				break;
			
			case 0b1111:
				/* 前回の結果により変える */
				switch (Last)
				{
					case -1:
						// スタート -> 直進
						forward(7, 3);
						break;
					
					default:
						// ゴール -> ループ終了
						Goal = true;
						break;
				}
				break;

			default:
				/* 危険な値なのでpass */
				__delay_us(50);
				break;

			two_ago = one_ago;
			one_ago = newest;
		}
	}
}
