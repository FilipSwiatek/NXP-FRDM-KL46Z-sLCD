/*  
	SLCD Library by Filip Swiatek
	Designed to use with FRDM KL46Z Board.
*/
#include "MKL46Z4.h"

uint32_t sLCD_Init(void); // sLCD initialization

uint8_t sLCD_Print(uint8_t digit, uint8_t number); // single digit print (0 to F) digits are numerated 3 down to 0

uint8_t sLCD_DotSet(uint8_t dot); // set dots (0 is colon, 1 is dot on 1st dec point etc)

uint8_t sLCD_DotClear(uint8_t dot); //clear dots and colon

void sLCD_Clear(void); //clear all LCD (also dots and colon)

uint8_t sLCD_Dec_Print(uint16_t value); //print decimal number from 0 to 9999

void sLCD_Hex_Print(uint16_t value); // print hexadecimal value from 0 to 0xFFFF
