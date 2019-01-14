#include <XC.h>
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
                forward(7, 10);
                break;
            
            // �����Ƀ��C�� -> ���ɋȂ���
            case 0b1110:
            case 0b1100:
            case 0b1000:
                M_L = M_LB = true;
                __delay_us(20);
                M_L = M_LB = false;
                turn_left(8, 10);
                Last = ZL;
                break;

            // �E���Ƀ��C�� -> �E�ɋȂ���
            case 0b0111:
            case 0b0011:
            case 0b0001:
                M_R = M_RB = true;
                __delay_us(20);
                M_R = M_RB = false;
                turn_right(8, 10);
                Last = ZR;
                break;

            // �O��̌��ʂɂ��ς���
            case 0b0100:
                switch (Last)
                {
                    // �����ɂ��郉�C���ɋ߂Â��Ă��� ( |\ ) -> �����E�ɋȂ���
                    case ZL:
                        turn_right(8, 10);
                        break;

                    // �E���ɂ��郉�C���ɋ߂Â��Ă��� ( /| ) -> �������ɋȂ���
                    case R:
                    case ZR:
                        turn_left(8, 10);
                        break;

                    // �X�^�[�g�̉\�������� -> ���i
                    default:
                        forward(7, 10);
                        Last = L;
                        break;
                }
                break;
            
            // �O��̌��ʂɂ��ς���
            case 0b0010:
                switch (Last)
                {
                    // �����ɂ��郉�C���ɋ߂Â��Ă��� ( |\ ) -> �����E�ɋȂ���
                    case ZL:
                    case L:
                        turn_right(8, 10);
                        break;

                    // �E���ɂ��郉�C���ɋ߂Â��Ă��� ( /| ) -> �������ɋȂ���
                    case ZR:
                        turn_left(8, 10);
                        break;
                    
                    // �X�^�[�g�̉\�������� -> ���i
                    default:
                        forward(7, 10);
                        Last = R;
                        break;
                }
                break;
            
            // �O��̌��ʂɂ��ς���
            case 0b0000:
                switch (Last)
                {
                    // �����Ƀ��C�� -> ���ɋȂ���
                    case ZL:
                    // �����ɂ��郉�C�����痣��Ă��� ( |/ ) -> �������ɋȂ���
                    case L:
                        turn_left(8, 10);
                        break;

                    // �E���Ƀ��C�� -> �E�ɋȂ���
                    case ZR:
                    // �E���ɂ��郉�C�����痣��Ă��� ( \| ) -> �����E�ɋȂ���
                    case R:
                        turn_right(8, 10);
                        break;

                    // �X�^�[�g�̉\�������� -> ���i
                    default:
                        forward(7, 10);
                        break;
                }
                break;
            
            // �O��̌��ʂɂ��ς���
            case 0b1111:
                switch (Last)
                {
                    // �X�^�[�g -> ���i
                    case -1:
                        forward(7, 10);
                        break;
                    
                    // �S�[�� -> ���[�v�I��
                    default:
                        //Goal = true;
                        break;
                }
                break;

            // �댯�Ȓl�Ȃ̂Ŋ����ňړ�
            default:
                __delay_us(50);
                break;
        }
        one_ago = newest;
    }
}
