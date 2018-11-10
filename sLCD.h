/*  
	SLCD Library by Filip Swiatek
	Designed to use with FRDM KL46Z Board.
*/

uint32_t sLCD_Init();

uint8_t sLCD_Print(uint8_t digit, uint8_t number);

uint8_t sLCD_DotSet(uint8_t dot);

uint8_t sLCD_DotClear(uint8_t dot);


void sLCD_Clear(void);

uint8_t sLCD_Dec_Print(uint16_t value);

void sLCD_Hex_Print(uint16_t value);