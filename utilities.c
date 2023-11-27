#include "utilities.h"

unsigned char HexCharToInt(char c){
	unsigned char ret = 0;
	if(c >= '0' && c <= '9'){
		ret = c - '0';
	} else if(c >= 'a' && c <= 'f'){
		ret = (c - 'a') + 10;
	} else if(c >= 'A' && c <= 'F'){
		ret = (c - 'A') + 10;
	}
	return ret;
}

unsigned char HexPairToInt(char low, char high){
	return (HexCharToInt(high) << 4) | HexCharToInt(low);
}

unsigned char DigitToLEDBits(char digit){
	switch(digit){
		case '0':
			return 0x3F;
		case '1':
			return 0x06;
		case '2':
			return 0x5B;
		case '3':
			return 0x4F;
		case '4':
			return 0x66;
		case '5':
			return 0x6D;
		case '6':
			return 0x7D;
		case '7':
			return 0x07;
		case '8':
			return 0x7F;
		case '9':
			return 0x6F;
		case '.':
			return 0x80;
	}
}