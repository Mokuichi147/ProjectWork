#include <stdio.h>

/* わかりやすくするため */
#define true  1
#define false 0

#define ZR 0
#define R  1
#define L  2
#define ZL 3

int M_L = false;
int M_R = false;
int M_RB = false;
int M_LB = false;

void turn (int L_ON, int R_ON, int TOTAL)
{
	if (L_ON)	M_L = true;
	if (R_ON)	M_R = true;
	for (int count=0; count<TOTAL; count++)
	{
		if (count == L_ON)	M_L = false;
		if (count == R_ON)	M_R = false;
		//printf("count:%d M_L:%d M_R:%d\n", count, M_L, M_R);
	}
}

void turn_left  (int ON_Time, int TOTAL) { turn(      0, ON_Time, TOTAL); }
void turn_right (int ON_Time, int TOTAL) { turn(ON_Time,       0, TOTAL); }
void forward    (int ON_Time, int TOTAL) { turn(ON_Time, ON_Time, TOTAL); }

int main (void)
{
	int Goal  = false;
	
	// 前回の結果を保持する
	int Last = -1;

	int input;
	int input_sub;

	// 誤差を抑える為に使う
	int newest;
	int one_ago = 0;
	int now;

	/* ライントレース */
	while (!Goal)
	{
		// 最新のセンサーの値を取得し、白を1にする (ラインがあると1)
		printf(">>> ");
		scanf("%d", &input);
		newest = 0;
		input_sub = 1;
		while (input > 0)
		{
			newest += (input % 10) * input_sub;
			input /= 10;
			input_sub *= 2;
		}
		now = newest & one_ago;

		switch (now)
		{
			// 本体の真ん中にライン -> 直進
			case 0b0110:
				puts("本体の真ん中にライン -> 直進");
				forward(7, 10);
				break;
			
			// 左側にライン -> 左に曲がる
			case 0b1110:
			case 0b1100:
			case 0b1000:
				M_L = M_LB = true;
				puts("M_L: ブレーキ");
				M_L = M_LB = false;
				puts("左側にライン -> 左に曲がる");
				turn_left(8, 10);
				Last = ZL;
				break;

			// 右側にライン -> 右に曲がる
			case 0b0111:
			case 0b0011:
			case 0b0001:
				M_R = M_RB = true;
				puts("M_R: ブレーキ");
				M_R = M_RB = false;
				puts("右側にライン -> 右に曲がる");
				turn_right(8, 10);
				Last = ZR;
				break;

			// 前回の結果により変える
			case 0b0100:
				puts("0100 -> 前回の結果により変える");
				switch (Last)
				{
					// 左側にあるラインに近づいている ( |\ ) -> 少し右に曲がる
					case ZL:
						puts("左側にあるラインに近づいている -> 少し右に曲がる");
						turn_right(8, 10);
						break;

					// 右側にあるラインに近づいている ( /| ) -> 少し左に曲がる
					case R:
					case ZR:
						puts("右側にあるラインに近づいている -> 少し左に曲がる");
						turn_left(8, 10);
						break;

					// スタートの可能性がある -> 直進
					default:
						puts("スタートの可能性がある -> 直進");
						forward(7, 10);
						Last = L;
						break;
				}
				break;
			
			// 前回の結果により変える
			case 0b0010:
				puts("0010 -> 前回の結果により変える");
				switch (Last)
				{
					// 左側にあるラインに近づいている ( |\ ) -> 少し右に曲がる
					case ZL:
					case L:
						puts("左側にあるラインに近づいている -> 少し右に曲がる");
						turn_right(8, 10);
						break;

					// 右側にあるラインに近づいている ( /| ) -> 少し左に曲がる
					case ZR:
						puts("右側にあるラインに近づいている -> 少し左に曲がる");
						turn_left(8, 10);
						break;
					
					// スタートの可能性がある -> 直進
					default:
						puts("スタートの可能性がある -> 直進");
						forward(7, 10);
						Last = R;
						break;
				}
				break;
			
			// 前回の結果により変える
			case 0b0000:
				puts("0000 -> 前回の結果により変える");
				switch (Last)
				{
					// 左側にライン -> 左に曲がる
					case ZL:
					// 左側にあるラインから離れている ( |/ ) -> 少し左に曲がる
					case L:
						printf("Last: %d 左に曲がる\n", Last);
						turn_left(8, 10);
						break;

					// 右側にライン -> 右に曲がる
					case ZR:
					// 右側にあるラインから離れている ( \| ) -> 少し右に曲がる
					case R:
						printf("Last: %d 右に曲がる\n", Last);
						turn_right(8, 10);
						break;

					// スタートの可能性がある -> 直進
					default:
						puts("スタートの可能性がある -> 直進");
						forward(7, 10);
						break;
				}
				break;
			
			// 前回の結果により変える
			case 0b1111:
				puts("1111 -> 前回の結果により変える");
				switch (Last)
				{
					// スタート -> 直進
					case -1:
						puts("スタート -> 直進");
						forward(7, 10);
						break;
					
					// ゴール -> ループ終了
					default:
						puts("ゴール -> ループ終了");
						//Goal = true;
						break;
				}
				break;

			// 危険な値なので慣性で移動
			default:
				puts("危険な値なので慣性で移動");
				break;
		}
		one_ago = newest;
	}
}