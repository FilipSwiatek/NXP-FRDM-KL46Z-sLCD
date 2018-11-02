/*  
	SLCD Library by Filip Swiatek
	Designed to use with FRDM KL46Z Board.
*/
#include "MKL46Z4.h"

//If  LCD doesn't work, uncomment  line below!
//MCG->C1  |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;

uint32_t LCD_Init()
{
// enabling required ports' clocks
	SIM->SCGC5|=SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTE_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5|=SIM_SCGC5_PORTC_MASK;
// enabling LCD controller clock
	SIM->SCGC5|=SIM_SCGC5_SLCD_MASK; 
// early initialising
	LCD->GCR|=LCD_GCR_PADSAFE_MASK; // turning on "safe mode" on all LCD pins in safe states
	LCD->GCR &= ~LCD_GCR_LCDEN_MASK; // turning off waveform generator
	
// setting pins MUXes to LCD
	PORTD->PCR[0] = PORT_PCR_MUX(0u);
	PORTE->PCR[4] = PORT_PCR_MUX(0u);
	PORTB->PCR[23] = PORT_PCR_MUX(0u);
	PORTB->PCR[22] = PORT_PCR_MUX(0u);
	PORTC->PCR[17] = PORT_PCR_MUX(0u);
	PORTB->PCR[21] = PORT_PCR_MUX(0u);
	PORTB->PCR[7] = PORT_PCR_MUX(0u);
	PORTB->PCR[8] = PORT_PCR_MUX(0u);
	PORTE->PCR[5] = PORT_PCR_MUX(0u);
	PORTC->PCR[18] = PORT_PCR_MUX(0u);
	PORTB->PCR[10] = PORT_PCR_MUX(0u);
	PORTB->PCR[11] = PORT_PCR_MUX(0u);
	
// general LCD Configuration
	LCD->GCR = 
	LCD_GCR_RVTRIM(0x00) |
	LCD_GCR_CPSEL_MASK | // charge pump settings
	LCD_GCR_LADJ(0x03) | // adjust to screen's capacitance
	LCD_GCR_VSUPPLY_MASK | // Vdd  inteernal/external setup
	LCD_GCR_ALTDIV(0x00) | 
	LCD_GCR_SOURCE_MASK |
	LCD_GCR_LCLK(0x01) |  
	LCD_GCR_DUTY(0x03); // adjust duty cycle
	
// other LCD configuration
	LCD->AR = LCD_AR_BRATE(0x03); // LCD blinking rate
	LCD->FDCR=0U; //clearing Fault detect LCD register
	
// setting LCD enabled ALL OF THE PINS
	LCD->PEN[0] =
	LCD_PEN_PEN(1u<<19 )|	//P19
	LCD_PEN_PEN(1u<<18) |	//P18
	LCD_PEN_PEN(1u<<17) |	//P17
	LCD_PEN_PEN(1u<<7)  |	//P7
	LCD_PEN_PEN(1u<<8)  |	//P8
	LCD_PEN_PEN(1u<<10) |	//P10
	LCD_PEN_PEN(1u<<11);	//P11

	LCD->PEN[1]=
	LCD_PEN_PEN(1u<<8 )|	//P40
	LCD_PEN_PEN(1u<<20)|	//P52
	LCD_PEN_PEN(1u<<5) |	//P37
	LCD_PEN_PEN(1u<<21)|	//P53
	LCD_PEN_PEN(1u<<6);	//P38

// setting LCD enabled Backpannel pins // when set then backpannel actvated
	LCD->BPEN[0] = LCD_BPEN_BPEN(1U<<18) | LCD_BPEN_BPEN(1U<<19);
	LCD->BPEN[1] = LCD_BPEN_BPEN(1U<<20) | LCD_BPEN_BPEN(1U<<8);

// setting COMn waveforms by 8-bit registers 	
	LCD->WF8B[40] = (1<<0)|(1<<4);	//COM0
	LCD->WF8B[52] = (1<<1)|(1<<5);	//COM1
	LCD->WF8B[19] = (1<<2)|(1<<6);	//COM2
	LCD->WF8B[18] = (1<<3)|(1<<7);	//COM3
	
	//conf finishing
	LCD->GCR &= ~LCD_GCR_PADSAFE_MASK; // turning off safe state
	LCD->GCR |= LCD_GCR_LCDEN_MASK; // enabling LCD
	return 0;
}



//decoder arrays declaration & initialization (without awful macros for every segment)
const volatile uint8_t ABC[16] = {0xEE,0x66,0xCC,0xEE,0x66,0xAA,0xAA,0xEE,0xEE,0xEE,0xEE,0x22,0x88,0x66,0x88,0x88}; //macros for 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F (A-F used only to decode 10:15) 
const volatile uint8_t DEFG[16]= {0xBB,0x00,0x77,0x55,0xCC,0xDD,0xFF,0x00,0xFF,0xDD,0xEE,0xFF,0xBB,0x77,0xFF,0xEE}; //macros for 0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F (A-F used only to decode 10:15) 
const volatile uint8_t digit_ABC[4] = {11,38,8,17}; //WF8B indexes to refer to set ./:, C, B, A segments in 1st, 2nd, 3rd and 4th digit
const volatile uint8_t digit_DEFG[4]= {10,53,7,37}; //WF8B indexes to refer to set D, E, G, F segments in 1st, 2nd, 3rd and 4th digit

	
uint8_t LCD_Print(uint8_t digit, uint8_t number)
{
	if(digit>3) return 1u; //error , LCD on board has only 4 digits (0:3)
	if(number>15) return 2u; //only 0:15 is decodable by this function 
	LCD->WF8B[digit_ABC[digit]] =ABC[number]; //saving setup to SLCD registers
	LCD->WF8B[digit_DEFG[digit]]=DEFG[number];//saving setup to SLCD registers
	return 0u; //error control	
}

uint8_t LCD_DotSet(uint8_t dot)
{
	if(dot>3) return 1; // error, non correct dotpoint
	LCD->WF8B[digit_ABC[dot]]|=0x11;
	return 0;
}

uint8_t LCD_DotClear(uint8_t dot)
{
	if(dot>3) return 1; // blad - zle miejsce kropki
	LCD->WF8B[digit_ABC[dot]]&=~0x11;
	return 1;
}


void LCD_Clear(void)
{
	for(int i=0;i<15;i++) LCD->WF [i]=0U; // clearing all WF registers
}

uint8_t LCD_Dec_Print(uint16_t value)
{
	if(value>9999) return 1;
	uint8_t tenpowers[4]={0,0,0,0};
	
	while(value>=1000)
	{
		tenpowers[3]++;
		value=value-1000;
	}
	while(value>=100)
	 {
		tenpowers[2]++;
		value=value-100;
	}
	 while(value>=10)
	 {
		tenpowers[1]++;
		value=value-10;
	}
	 tenpowers[0]=value;
	 
	for(uint8_t i=0;i<4;i++)
	{
		LCD_Print(i,tenpowers[i]);
	}
return 0;
}

void LCD_Hex_Print(uint16_t value)	
{
	for(int i=0;i<4;i++)
	{
	LCD_Print(i,(value>>(i*4))&0xF);
	}
}



