#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#define F_CPU 16000000UL // 16 MHz
#define sbi(PORTX,bitX)PORTX|=(1<<bitX)
#define cbi(PORTX,bitX)PORTX&=~(1<<bitX)
#define DO 0
#define RE 1
#define MI 2
#define FA 3
#define SOL 4
#define RA 5
#define SI 6
#define DDO 7
#define EOS -1 // End Of Song  표시
#define ON 0
#define OFF 1
#define LEFT 1
#define CENTER 2
#define RIGHT 3

volatile int tone;

char f_table[8]={ 17,43,66,77,97,114,117,137};

// 도레미파솔라시도 에 해당하는 TCNT0 값을 미리 계산해 놓은 값

int correctsound[]={ DO,MI,SOL,DDO,EOS}; // 원하는 노래 계이름 끝에 EOS 잊지말고 넣기 이건 open
int failsound[]={ DO,DO,DO,DO,EOS}; // 이건 fail

/*도트 포트 핀
G0   PORTG 0
G1   PORTG 1
G2   PORTG 2
G3   PORTG 3
SDI(RED)PORTF 0
SDI(GREEN)PORTF 1
CLK   PORTF 4
LE    PORTF 5
OUT   GND
GND   GND
*/

//FAIL
unsigned char fail[16][32]={

	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,1,1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
	{ 0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

//open

unsigned char open[16][32]={

	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,2,2,2,2,0,0,0,0,2,2,2,2,2,0,0,0,2,2,2,2,0,0,0,0,2,2,2,2,2,0},
	{ 0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0},
	{ 0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0},
	{ 0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0},
	{ 0,2,0,0,0,0,2,0,0,0,2,2,2,2,2,0,0,0,2,2,2,2,2,0,0,2,0,0,0,0,2,0},
	{ 0,2,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,0},
	{ 0,2,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,2,0},
	{ 0,2,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,2,0},
	{ 0,2,0,0,0,0,2,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,2,0,0,2,0,0,0,0,2,0},
	{ 0,0,2,2,2,2,0,0,0,0,0,0,0,0,2,0,0,0,2,2,2,2,0,0,0,2,0,0,0,0,2,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

};

//lock

unsigned char lock[16][32]={

	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,3,0,0,0,3,3,3,3,0,0,0,0,3,3,3,3,0,0,0,0,3,0,0,0,3,0},
	{ 0,0,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,0,0,3,0,0,3,0},
	{ 0,0,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,3,0,3,0},
	{ 0,0,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,3,0},
	{ 0,0,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,3,0},
	{ 0,0,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,3,0},
	{ 0,0,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,3,0,3,0},
	{ 0,0,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,3,0,0,0,0,3,0,0,0,0,3,0,0,3,0},
	{ 0,0,3,3,3,3,3,0,0,0,3,3,3,3,0,0,0,0,3,3,3,3,0,0,0,0,3,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

};

char data;

char string1[16]="   Success!!   ";
char string2[16]="     OPEN!     ";
char string3[16]="     Fail.     ";
char string4[16]="     Retry.    ";
char string5[16]="    DoorLock   ";
char string6[16]="   By YDG  ";

char buf[32]={ ' ',};

int value=666;

ISR(TIMER0_OVF_vect)// Timer/Counter0 오버플로우 인터럽트
{
	TCNT0=f_table[tone];
	PORTB^=0x10;
}

//
void LCD_command(char command){

    _delay_us(100);
	PORTA=(command&0xF0); // send High data
	cbi(PORTA,0); // RS=0
	//cbi(PORTA,1); // RW=0
	sbi(PORTA,2); // Enable
	_delay_us(1);
	cbi(PORTA,2); // Disable
	PORTA=(command&0x0F)<<4; // send Low data
	cbi(PORTA,0); // RS=0
	//cbi(PORTA,1); // RW=0
	sbi(PORTA,2); // Enable
	_delay_us(1);
	cbi(PORTA,2); // Disable
}

//
void LCD_data(char data){

	_delay_us(100);
	PORTA=(data&0xF0); // send High data
	sbi(PORTA,0); // RS=1
	//cbi(PORTA,1); // RW=0
	sbi(PORTA,2); // Enable
	_delay_us(1);
	cbi(PORTA,2); // Disable
	PORTA=(data&0x0F)<<4; // send Low data
	sbi(PORTA,0); // RS=1
	//cbi(PORTA,1); // RW=0
	sbi(PORTA,2); // Enable
	_delay_us(1);
	cbi(PORTA,2); // Disable

}

//
void LCD_init(void)
{

	_delay_ms(50);
	LCD_command(0x28); // DL=0(4bit)N=1(2Line)F=0(5x7)
	_delay_ms(2); //[function set]0b00101000
	// 4:(DL)1이면 8bit모드,0이면 4bit모드
	// 3:(N)0이면 1줄짜리,1이면 2줄짜리
	// 2:(F)0이면 5x8dots,1이면 5x11dots
	LCD_command(0x0C); // LCD ON,Cursor X,Blink X
	_delay_ms(2); //[display on/off control]0b00001100
	// 2:(D)1이면 display on,0이면 off
	// 1:(C)1이면 cursor on,0이면 off
	// 0:(B)1이면 cursor blink,0이면 off
	LCD_command(0x06); // Entry Mode
	_delay_ms(2); //[entry mode set]0b00000110
	// 1:(I/D)1이면 오른쪽으로,0이면 왼쪽
	// 0:(SH)CGRAM 사용관련
	LCD_command(0x01); // LCD Clear
	_delay_ms(2);

}

//
void LCD_string(char line,char*string)

{
	LCD_command(0x80+((line-1)*0x40));
	while(*string)
	LCD_data(*string++);
}

//
void servo(int position) // 전체 주기 = 19ms < 20ms
{
	if (position == LEFT)
	{ PORTE |= 1<<7; _delay_us(1500); PORTE &= ~(1<<7); _delay_ms(18); _delay_us(1500); }
	else if (position == CENTER)
	{ PORTE |= 1<<7; _delay_ms(1); _delay_us(1500); PORTE &= ~(1<<7);  _delay_ms(18); _delay_us(1500);  }
	else
	{ PORTE |= 1<<7; _delay_ms(2); _delay_us(1500); PORTE &= ~(1<<7);  _delay_ms(16); _delay_us(700); }
}

//
int main(void){

	int i,j;
	DDRA=0xff; DDRB=0x10; DDRG=0x0f; DDRF=0xff; DDRE = 0xff;
	UCSR1B=0x18; UBRR1L=103; // 9600 @16MHz

	LCD_init();

	while(1){

		if(UCSR1A&0x80){

			data=UDR1;

			if(data=='2'){ //해당 지문 없음

				int x=0;

				TCCR0=0x03; TIMSK=0x01; TCNT0=f_table[failsound[x]]; sei();
				do{ tone=failsound[x++]; _delay_ms(1700); } while(tone!=EOS);

                sprintf(buf,"%s ",string3); 				
				LCD_string(1,buf); _delay_ms(2);
                LCD_string(2,string4); _delay_ms(2);
				
				int q=0;

				for(q=0; q<200; q++){
					for(i=0; i<16; i++){
						PORTG=i; // 행
						for(j=0; j<32; j++){ PORTF=fail[i][j]; PORTF|=0x10; PORTF&=~0x10; } //열&클록
						PORTF|=0x20; PORTF&=~0x20; _delay_ms(1); // 래치
					}
				}		
			}

			else if(data=='1'){ //등록 지문 일치

				int x=0;

				TCCR0=0x03; TIMSK=0x01; TCNT0=f_table[correctsound[x]]; sei();
				do{ tone=correctsound[x++]; _delay_ms(1700); } while(tone!=EOS);

                sprintf(buf,"%s ",string1); LCD_string(1,buf); _delay_ms(2);
                LCD_string(2,string2); _delay_ms(2);
				
				int y;//서보모터
				for (y=0; y<80; y++) servo(LEFT);
				
				int q=0;
				for(q=0; q<200; q++){
					for(i=0; i<16; i++){
						PORTG=i;
						for(j=0; j<32; j++){ PORTF=open[i][j]; PORTF|=0x10; PORTF&=~0x10; }
						PORTF|=0x20; PORTF&=~0x20; _delay_ms(1);
					}
				}
				for (y=0; y<80; y++) servo(CENTER);
			}

			else if(data=='0'){ //잠김 상태

				PORTB=0x00;
				
				sprintf(buf,"%s ",string5); LCD_string(1,buf); _delay_ms(2);
				LCD_string(2,string6); _delay_ms(2);
				
				int q=0;
              for(q=0; q<100; q++){
				  for(i=0; i<16; i++){
					PORTG=i; // 행
					for(j=0; j<32; j++){ PORTF=lock[i][j]; PORTF|=0x10; PORTF&=~0x10; } //열&클록
					PORTF|=0x20; PORTF&=~0x20; _delay_ms(1); // 래치
					}
			   }
              
			}

		}

	}

}

