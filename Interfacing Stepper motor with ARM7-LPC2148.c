//Interfacing STEPPER MOTOR with ARM7-LPC2148
//CIRCUIT DIGEST
//Code by Pramoth.T

#include<lpc214x.h>

void delay(unsigned int value);

unsigned char clockwise[4] = {0x1,0x2,0x4,0x8};     //Commands for clockwise rotation
unsigned char anticlockwise[4] = {0x8,0x4,0x2,0x1}; //Commands for anticlockwise rotation

int no_of_steps = 550;  //Change this value for required number of steps rotation (550 gives one complete rotation)

int main()
{
	
	
	PINSEL0 = 0x00000000; //Setting PORT0 pins 
	IO0DIR |= 0x00000780; //Setting pins P0.7, P0.8, P0.9, P0.10 as OUTPUT
	IO0CLR = 0x00000780;  //Setting P0.7, P0.8, P0.9, P0.10 pins OUTPUT as LOW
	
	while(1)              // While loop for continueous operation
 {
	for (int j=0; j<no_of_steps;j++)     
	{
		for(int i=0; i<4;i++)
		{
			IOPIN0 =clockwise[i]<<7;  // Settting the pin value HIGH one by one after shifting bit to left
			delay(0x10000);           //Change this value to change the speed of rotation
			}
	}
	for (int z=0;z<no_of_steps;z++)   
	{
		for(int i=0; i<4;i++)
		{
			IOPIN0 =anticlockwise[i]<<7;
			delay(0x10000);            //Change this value to change the speed of rotation
			}
	}
	
}
 }


void delay(unsigned int value)       //Function to generate delay
{
	unsigned int z;
	for(z=0;z<value;z++);
}