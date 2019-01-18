#include <xc.h>
#include <pic16f84a.h>

#pragma config FOSC  = HS
#pragma config WDTE  = OFF
#pragma config PWRTE = ON

#define _XTAL_FREQ 10000000

/* �킩��₷�����邽�� */
#define true  1
#define false 0

#define ZR 0
#define R  1
#define L  2
#define ZL 3

// ���[�^�[		��: �E�֋Ȃ��� ->  M_R = false, M_L = true
#define M_R  PORTBbits.RB7
#define M_L  PORTBbits.RB4
#define M_RB PORTBbits.RB6
#define M_LB PORTBbits.RB5

// __delay_us�̎���
#define time_us 1500

/* ���[�^�[�𓮂����ׂ̊֐� */
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

    // �O��̒l�Ɋ�Â��{�̂̒��S�Ƀ��C��������悤�ɂ���
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

    // �X�^�[�g�{�^���ƃS�[���̃t���O
    int Start = false;
    int Goal = false;
    // �O��̌��ʂ�ێ�����
    int Last = -1;
    // �덷��}����ׂɎg��
    int newest;
    int one_ago = 0;
    int now;
    
    /* �X�^�[�g�{�^�� */
    while (PORTAbits.RA4 || !Start)
        Start = PORTAbits.RA4;
    
    /* ���C���g���[�X */
    while (!Goal)
    {
        // �ŐV�̃Z���T�[�̒l���擾���A����1�ɂ��� (���C���������1)
        newest = ~PORTA & 0b1111;
        now = newest & one_ago;
        
        switch (now)
        {
            // �{�̂̐^�񒆂Ƀ��C�� -> ���i
            case 0b0110:
                forward(10, 10);
                break;
            
            // �����Ƀ��C�� -> ���ɐ��񂵃��C���𒆐S�ɖ߂�
            case 0b1000:
            case 0b1100:
            case 0b1110:
                TURN(ZL);
                Last = ZL;
                break;
            
            // �E���Ƀ��C�� -> �E�ɐ��񂵃��C���𒆐S�ɖ߂�
            case 0b0001:
            case 0b0011:
            case 0b0111:
                TURN(ZR);
                Last = ZR;
                break;
            
            // �����Ƀ��C�� -> �������ɋȂ���
            case 0b0100:
                PWM(7, 10, 10);
                Last = L;
                break;
            
            // �E���Ƀ��C�� -> �����E�ɋȂ���
            case 0b0010:
                PWM(10, 7, 10);
                Last = R;
                break;
            
            // �O��̒l�Ɋ�Â����S�Ƀ��C��������悤�ɒ���
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
            
            // �X�^�[�g�ƃS�[���A90�x�J�[�u�̎�
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