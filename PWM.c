//PWM using ARM7-LPC2148 (Controlling the brigtness of LED)
//CIRCUIT DIGEST
//Pramoth.T

#include <lpc214x.h> 
#include <stdint.h>
#include <string.h>

void initilizePLL(void);
void initilizePWM(unsigned int periodPWM);
void delaytime(uint16_t j);

void initilizePLL (void)     //Function to use PLL for clock generation
{
  PLL0CON = 0x01;           
  PLL0CFG = 0x24;
  PLL0FEED = 0xAA;
  PLL0FEED = 0x55;
  while(!(PLL0STAT & 0x00000400));
  PLL0CON = 0x03;
  PLL0FEED = 0xAA;
  PLL0FEED = 0x55;
  VPBDIV = 0x01;
} 

void delaytime(uint16_t j)      // fucntion to generate 1 milisecond delay
{
    uint16_t x,i;
	for(i=0;i<j;i++)
	{
    for(x=0; x<6000; x++);    
	}
}


void initilizePWM(unsigned int PWMvalue)        
{
  PINSEL0 = 0x00000002;         //Setting pin P0.0 for PWM output
  PWMTCR = (1<<1);              //Setting PWM Timer Control Register as counter reset
  PWMPR = 0X00;                 //Setting PWM prescale value
  PWMMCR = (1<<0)|(1<<1);       //Setting PWM Match Control Register
  PWMMR0 = PWMvalue;            //Giving PWM value Maximum value
  PWMLER = (1<<0);              //Enalbe PWM latch
  PWMTCR = (1<<0) | (1<<3);      //Enabling PWM and PWM counter

}

void LCD_SEND(char command)     //Function to send hex commands 
{
IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((command & 0xF0)<<8) ); //Send upper nibble of command 
IO0SET = 0x00000040; //Making Enable HIGH
IO0CLR = 0x00000030; //Making RS & RW LOW
delaytime(5);
IO0CLR = 0x00000040; //Makeing Enable LOW
delaytime(5);
IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((command & 0x0F)<<12) ); //Send Lower nibble of command 
IO0SET = 0x00000040;  //ENABLE HIGH
IO0CLR = 0x00000030;  //RS & RW LOW
delaytime(5);
IO0CLR = 0x00000040;  //ENABLE LOW
delaytime(5);
}
 
void LCD_INITILIZE(void)          //Function to get ready the LCD
{
IO0DIR = 0x0000FFF0; //Sets pin P0.12,P0.13,P0.14,P0.15,P0.4,P0.6 as OUTPUT
delaytime(20);
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
delaytime(2);
IO0CLR = 0x00000040; // EN = 0, RS and RW unchanged(i.e. RS = 1, RW = 0) 
delaytime(5);
IO0PIN = ( (IO0PIN & 0xFFFF00FF) | ((msg[i] & 0x0F)<<12) ); //Sends Lower nibble
IO0SET = 0x00000050; //RS & EN HIGH
IO0CLR = 0x00000020; 
delaytime(2);
IO0CLR = 0x00000040; 
delaytime(5);
i++;
}
}

int main()
{
	LCD_INITILIZE();                //Calls function to get ready the LCD to display
	char displayadc[18];
	float brightness;
	char ledoutput[18];
        LCD_DISPLAY("CIRCUIT DIGEST");
	delaytime(900); 
	LCD_SEND(0xC0);
	LCD_DISPLAY("PWM IN LPC2148");
	delaytime(900); 
	
	int dutycycle;
        float adcvalue;       
        initilizePLL();                 //Calls function initilizePLL
        PINSEL1 = 0x01000000;           //Setting P0.28 as ADC INPUT
        AD0CR = (((14)<<8) | (1<<21)); //Setting clock and PDN for A/D Conversion
        initilizePWM(255);             //Calls function initilizePWM with value 255       
        PWMPCR |= (1<<9);              //To enable PWM1 output at pin P0.0 of LPC2148 
  
while(1)
  {
    AD0CR |= (1<<1);                   //Select AD0.1 channel in ADC register
    delaytime(10);               
    AD0CR |= (1<<24);                  //Start the A/D conversion
    while( (AD0DR1 & (1<<31)) == 0 );  //Check the DONE bit in ADC Data register
    adcvalue = (AD0DR1>>6) & 0x3ff;     //Get the RESULT from ADC data register
    dutycycle = adcvalue/4;             //formula to get dutycycle values from (0 to 255)
    PWMMR1 = dutycycle;                 //set dutycycle value to PWM match register 
    PWMLER |= (1<<1);              	//Enable PWM output with dutycycle value
    brightness = dutycycle;
    LCD_SEND(0x80);
    sprintf(displayadc, "adcvalue=%f", adcvalue); 
    LCD_DISPLAY(displayadc);		               //Display ADC value (0 to 1023)
    LCD_SEND(0xC0);  
    sprintf(ledoutput, "PWM OP=%.2f   ", brightness);  
    LCD_DISPLAY(ledoutput);                           //Display dutycycle values from (0 to 255)
}

}
