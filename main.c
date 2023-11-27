/*------------------------------------------------------------------------------
HELLO.C - adapted for the P89LPC932 and the Keil MCB900 Microcontroller Board

Copyright 1995-2005 Keil Software, Inc.
------------------------------------------------------------------------------*/

#include <REG922.H>               /* special function register declarations   */
                                  /* for the Philips P89C932 device           */
#include "utilities.h"

#define CONF_CMD_BUF_SIZE 12

//":B AABBCCDD"
char cmd_buffer[CONF_CMD_BUF_SIZE + 1];
char cmd_index = 0;

struct DisplayBits {
		unsigned char digit_0;
	  unsigned char digit_1;
		unsigned char digit_2;
	  union {
			unsigned char c;
			struct {
				unsigned char neg : 1;
				unsigned char celcius : 1;
				unsigned char dot_celcius : 1;
				unsigned char dot_0 : 1;
				unsigned char dot_1 : 1;
				unsigned char dot_2 : 1;
				unsigned char dot_3 : 1;
			} bits;
		} misc;

} display;

struct Context {
	unsigned char cmd_toggle : 1;
	unsigned char cmd_read : 1;
} context;

void isr(void) interrupt 4
{
	char c;
  
	RI = 0;
  c = SBUF;
	if(context.cmd_read){
		if(cmd_index < CONF_CMD_BUF_SIZE){
			cmd_buffer[cmd_index++] = c;
			cmd_buffer[cmd_index] = 0;
		}
	} else if( c == ':' ){
		context.cmd_read = 1;
	}
}

/*
#pragma vector=serial_tx
__interrupt void tx_interrupt(){  
}
*/
void InitUART(){
  ES = 1;
  EA = 1;
  
  SCON = 0x50;									// init UART
  SSTAT = 0x60; // separate rx interrupt
  
  //9600
  //BRGR0 = 0xF0;									
  //BRGR1 = 0x02;		
  
  //115200
  BRGR0 = 48;
  BRGR1 = 0;
  
  BRGCON = 0x03;								// internal RC oscillator  
  TI = 0;
  RI = 0;
  
  
  
}

void InitDisplay(){
	int i = 0;
  display.digit_0 = 0;
	display.digit_1 = 0;
	display.digit_2 = 0;
	
  display.misc.bits.neg = 0;
  display.misc.bits.celcius = 0;
  display.misc.bits.dot_celcius = 0;
  display.misc.bits.dot_0 = 0;
  display.misc.bits.dot_1 = 0;
  display.misc.bits.dot_2 = 0;
  display.misc.bits.dot_3 = 0;
}

void InitContext(){
	context.cmd_read = 0;
}

void ResetDisplay(){
  P0M1 = 0xFF;
  P0M2 = 0x00;
  P1M1 |= 0xC0;
  P1M2 &= 0x3F;
  P0 = 0xFF;
  P1 |= 0xC0;
}

void SetLow(int index){
  switch(index){
  case 0:
    P0M1 &= 0xFE;
    P0M2 |= 0x01;
    P0 &= 0xFE;
    break;
  case 1:
    P0M1 &= 0xFD;
    P0M2 |= 0x02;
    P0 &= 0xFD;
    break;
  case 2:
    P0M1 &= 0xFB;
    P0M2 |= 0x04;
    P0 &= 0xFB;
    break;
  case 3:
    P0M1 &= 0xF7;
    P0M2 |= 0x08;
    P0 &= 0xF7;
		break;
  case 4:
    P0M1 &= 0xBF;
    P0M2 |= 0x40;
    P0 &= 0xBF;
    break;
  case 5:
    P0M1 &= 0x7F;
    P0M2 |= 0x80;
    P0 &= 0x7F;
    break;
  case 6:
    P1M1 &= 0x7F;
    P1M2 |= 0x80;
    P1 &= 0x7F;
    break;
  }
}

void SetHigh(int index){
  switch(index){
  case 0:
    P0M1 &= 0xFE;
    P0M2 |= 0x01;
    P0 |= 0x01;
    break;
  case 1:
    P0M1 &= 0xFD;
    P0M2 |= 0x02;
    P0 |= 0x02;
    break;
  case 2:
    P0M1 &= 0xFB;
    P0M2 |= 0x04;
    P0 |= 0x04;
    break;
  case 3:
    P0M1 &= 0xF7;
    P0M2 |= 0x08;
    P0 |= 0x08;
    break;
  case 4:
    P0M1 &= 0xBF;
    P0M2 |= 0x40;
    P0 |= 0x40;
    break;
  case 5:
    P0M1 &= 0x7F;
    P0M2 |= 0x80;
    P0 |= 0x80;
    break;
  case 6:
    P1M1 &= 0x7F;
    P1M2 |= 0x80;
    P1 |= 0x80;
    break;
  }
}

void EnableDisplay(int index_a, int index_b){
  ResetDisplay();
  if(index_a != index_b && index_a < 7 && index_b < 7){
    SetLow(index_a);
    SetHigh(index_b);
  }
}

void SetDisplay(char enable, int index_a, int index_b){
  ResetDisplay();
  
  if(enable){
    if(index_a != index_b && index_a < 7 && index_b < 7){
      SetLow(index_a);
      SetHigh(index_b);
    }
  }
  //if(updater)
    //updater();
}

void UpdateDisplay(){
  //Digit 1
  SetDisplay(display.digit_0 & 1, 0, 1);
  SetDisplay(display.digit_0 & 2, 0, 2);
  SetDisplay(display.digit_0 & 4, 3, 0);
  SetDisplay(display.digit_0 & 8, 4, 0);
  SetDisplay(display.digit_0 & 16, 0, 3);
  SetDisplay(display.digit_0 & 32, 1, 0);
  SetDisplay(display.digit_0 & 64, 2, 0);
  SetDisplay(display.digit_0 & 128, 4, 5);
  
  //Digit 2
  SetDisplay(display.digit_1 & 1, 1, 2);
  SetDisplay(display.digit_1 & 2, 1, 3);
  SetDisplay(display.digit_1 & 4, 4, 1);
  SetDisplay(display.digit_1 & 8, 1, 5);
  SetDisplay(display.digit_1 & 16, 1, 4);
  SetDisplay(display.digit_1 & 32, 2, 1);
  SetDisplay(display.digit_1 & 64, 3, 1);
  SetDisplay(display.digit_1 & 128, 2, 5);
  
  //Digit 3
  SetDisplay(display.digit_2 & 1, 4, 3);
  SetDisplay(display.digit_2 & 2, 2, 4);
  SetDisplay(display.digit_2 & 4, 3, 4);
  SetDisplay(display.digit_2 & 8, 5, 0);
  SetDisplay(display.digit_2 & 16, 5, 2);
  SetDisplay(display.digit_2 & 32, 3, 2);
  SetDisplay(display.digit_2 & 64, 4, 2);
  SetDisplay(display.digit_2 & 128, 2, 3);
  
  SetDisplay(display.misc.bits.neg, 5, 3);
  
  SetDisplay(display.misc.bits.dot_celcius, 3, 5);
  SetDisplay(display.misc.bits.celcius, 5, 4);
  
  SetDisplay(display.misc.bits.dot_0, 6, 5);
  SetDisplay(display.misc.bits.dot_1, 5, 6);
  SetDisplay(display.misc.bits.dot_2, 4, 6);
  SetDisplay(display.misc.bits.dot_3, 6, 4);
  
}

void Print(const char *s){
	char* i = s;
  //while(!SCON_bit.TI);
  //SCON_bit.TI = 0;
  //for(int i = 0; s[i] != 0; ++i){
  for(; *i != 0; ++i){
    //while(!SCON_bit.RI);									// wait until reception is complete
    //SCON_bit.RI = 0;		
    //char ch = SBUF;
    SBUF = *i;
    while(!TI);
    TI = 0;
  }
}

void Delayer(){
	int i = 0;
  for(; i < 100; ++i);
}

void DisplayFloatString(char* str){
	int i = 0;
	for(i = 0; i < 3; ++i){
		if(!((str[i] >= '0' && str[i] <= '9')))
			break;
	}
	--i;
	if(i >= 0)
		display.digit_2 = DigitToLEDBits(str[i--]);
	
	if(i >= 0)
		display.digit_1 = DigitToLEDBits(str[i--]);
	
	if(i >= 0)
		display.digit_0 = DigitToLEDBits(str[i--]);
}

void CheckCommand(){
	if(cmd_index > 0){
		switch(cmd_buffer[0]){
			//B AABBFFDD
			case 'B':
				if(cmd_index >= 10){
					
					display.digit_0 = HexPairToInt(cmd_buffer[3], cmd_buffer[2]);
					display.digit_1 = HexPairToInt(cmd_buffer[5], cmd_buffer[4]);
					display.digit_2 = HexPairToInt(cmd_buffer[7], cmd_buffer[6]);
					display.misc.c = HexPairToInt(cmd_buffer[9], cmd_buffer[8]);
					
					cmd_index = 0;
					context.cmd_read = 0;
				}
				break;
			//F 0.0
		  //F +0.0
			//F -0.0
			//F 0
			case 'F': {
				char* p = cmd_buffer + 2;
				display.misc.bits.neg = 0;
				display.digit_0 = display.digit_1 = display.digit_2 = 0;
				
				if(*p == '-'){
					display.misc.bits.neg = 1;
					++p;
				} else if(*p == '+')
					++p;
				DisplayFloatString(p);
				
				cmd_index = 0;
				context.cmd_read = 0;
			}
				break;
		}
	}
}

int main(){
	int x = 0, y = 0, z = 0;
	
  P0M1 = 0xFC;									// P0 in input only mode if no activity	
  P0M2 = 0x03;	
  P3M1 = 0xFF;
  P3M2 = 0x00;
  
  //P1M1 = 0xF2;
  //P1M2 = 0x0C;
  
  //P1 = 0x04;
  
  P1M1 = 0xFE;
  P1M2 = 0x00;
  
  P0 = 0xFD;
  
  InitUART();
  InitDisplay();
  InitContext();
	
  //display.digit[0] = 0x3F;
  //display.digit[1] = 0x3F;
	//display.digit[2] = 0xBF;
  for(;;){
		//uart_received_int = 1;
    //if(uart_received_int){
      //SBUF = uart_received; 
      //while(!TI);
      //TI = 0;
      //uart_received_int = 0;
    //}
   
    CheckCommand();
    UpdateDisplay();
  }
  return 0;
  
}