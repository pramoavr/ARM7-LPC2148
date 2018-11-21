//Using ADC in ARM7-LPC2148
//Circuit Digest

#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void delay_ms(uint16_t j)  // Function for making delay in milliseconds 
{
    uint16_t x,i;
for(i=0;i<j;i++)
{
    for(x=0; x<6000; x++);    // Forloop to generate 1 millisecond delay with Cclk = 60MHz 
}
}
 
void LCD_SEND(char command)     //Function to send hex commands 
{
IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((command & 0xF0)<<8) ); //Send upper nibble of command 
IO0SET = 0x00000040; //Making Enable HIGH
IO0CLR = 0x00000030; //Making RS & RW LOW
delay_ms(5);
IO0CLR = 0x00000040; //Makeing Enable LOW
delay_ms(5);
IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((command & 0x0F)<<12) ); //Send Lower nibble of command 
IO0SET = 0x00000040;  //ENABLE HIGH
IO0CLR = 0x00000030;  //RS & RW LOW
delay_ms(5);
IO0CLR = 0x00000040;  //ENABLE LOW
delay_ms(5);
}
 
void LCD_INITILIZE(void)          //Function to get ready the LCD
{
IO0DIR = 0x0000FFF0; //Sets pin P0.12,P0.13,P0.14,P0.15,P0.4,P0.6 as OUTPUT
delay_ms(20);
LCD_SEND(0x02);  // Initialize lcd in 4-bit mode of operation
LCD_SEND(0x28);  // 2 lines (16X2)
LCD_SEND(0x0C);   // Display on cursor off 
LCD_SEND(0x06);  // Auto increment cursor 
LCD_SEND(0x01);   // Display clear 
LCD_SEND(0x80);  // First line first position 
}
 
void LCD_DISPLAY (char* msg)         //Function to print the characters sent one by one 
{
uint8_t i=0;
while(msg[i]!=0)
{
IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((msg[i] & 0xF0)<<8) ); //Sends Upper nibble 
IO0SET = 0x00000050;  //RS HIGH & ENABLE HIGH to print data 
IO0CLR = 0x00000020;  //RW LOW Write mode
delay_ms(2);
IO0CLR = 0x00000040; // EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) 
delay_ms(5);
IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((msg[i] & 0x0F)<<12) ); //Sends Lower nibble
IO0SET = 0x00000050; //RS & EN HIGH
IO0CLR = 0x00000020; 
delay_ms(2);
IO0CLR = 0x00000040; 
delay_ms(5);
i++;
}
}

int main(void)
{
        LCD_INITILIZE();        //Calls function to get ready the LCD to display
	uint32_t adcvalue;
	float voltage;
	char voltvalue[18];
        LCD_DISPLAY("CIRCUIT DIGEST");
	delay_ms(900); 
	PINSEL1 = 0x01000000; // Select P0.28 as AD0.1
	AD0CR = 0x00200402;  //Sets ADC operation as 10-bits/11 CLK for conversion 
	while(1)                                //while loop that excecutes continueously
	{
		AD0CR = AD0CR | (1<<24); // Starts Conversion
		while ( !(AD0DR1 & 0x80000000) ); // Waits for DONE 
		adcvalue = AD0DR1;
		adcvalue = (adcvalue>>6);             //Right Shift six bits 
		adcvalue = (adcvalue & 0x000003FF);      
		voltage = ( (adcvalue/1023.0) * 3.3 ); // formula to convert ADC value into voltage 
		LCD_SEND(0x80);                            
		LCD_DISPLAY("ADC WITH LPC2148");
		LCD_SEND(0xC0); 
		sprintf(voltvalue, "Voltage=%.2f V  ", voltage);
		LCD_DISPLAY(voltvalue);
		memset(voltvalue, 0, 18);
	}
}