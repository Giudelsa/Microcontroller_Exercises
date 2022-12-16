/***************************************
 *
 * FileName:        main.c
 * Processor:       PIC18F25K50
 * Compiler:        MPLAB®️ XC8
 * Comment:         Main code
 * Dependencies:    Header (.h) files if applicable, see below
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Author                       Date                Version             Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Eva Andries	                20/08/2018          0.1                 Initial release
 * Tim Stas						15/07/2019			1.0					PIC18F25K50
 * G.S.							6/10/2020			1.1					CLK
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * TODO                         Date                Finished
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 ***************************************/

/*
 * Includes
 */
#include <xc.h>



/*
 * Prototypes
 */
void initChip(void);

void __interrupt (high_priority) high_ISR(void);   //high priority interrupt routine
void __interrupt (low_priority) low_ISR(void);

/*
 * Global Variables
 */
char counter = 0; //used to set the value of CCPR2L
char up = 1; //used as a boolean to know whether we go up(1) or down(0) with the duty cycle value


/*
 * Functions
 */

void __interrupt (high_priority) high_ISR(void)
{
    if(PIR1bits.TMR2IF == 1) //interrupt caused by the TMR
    {
      if(PORTAbits.RA1 == 1) //switch : Triangle wave
      { 
       if(up == 1)
       {
         counter++;
         if(counter == 74) 
          {
             up = 0;
          }
       }
    
       else if(up == 0)
       {
          counter--;
          if(counter <= 0)
          {
             up = 1;
          }
       }
      }//end of switch

     else if(PORTAbits.RA1 == 0) //switch : Sawtooth wave
     {
         if(counter >= 74)
         {
            counter = 0;
         }
         counter++;

      }//end of switch
      
       
    }

    
    PIR1bits.TMR2IF = 0; //clear the flag
    TMR2=0x00; //reset the timer
}


void initChip(void)
{
    
	//CLK settings
	OSCTUNE = 0x80; //3X PLL ratio mode selected
	OSCCON = 0x70; //Switch to 16MHz HFINTOSC
	OSCCON2 = 0x10; //Enable PLL, SOSC, PRI OSC drivers turned off
	//while(OSCCON2bits.PLLRDY != 1); //Wait for PLL lock
	ACTCON = 0x90; //Enable active clock tuning for USB operation
        
        PORTA = 0x00; //Initial PORTA   
        TRISA = 0b00000001; //Define PORTA as input  
        ANSELA = 0b00000000; // define digital or analog
        ADCON1 = 0x00; //AD voltage reference           
        CM1CON0 = 0x00; //Turn off Comparator        
        PORTB = 0x00; //Initial PORTB 
        TRISB = 0x00; //Define PORTB as output
        PORTC = 0x00; //Initial PORTC
        TRISC = 0x00; //Define PORTC as output
    
}

void pwmMode(void)
{
    CCP2CON = 0b00001111;
    PR2 = 0x4A; //set PR2 at 74
    T2CON =0b00000110; //timer 2 initialization : prescale 16, timer on
    PIR1bits.TMR2IF = 0; //clear the flag bit
    PIE1bits.TMR2IE = 1;//enables the PWM interrupt
    INTCON= 0b11000000;
    //TRISCbits.RC1 = 0;   //define portC as output
    
    TMR2 = 0x00; 
}



void main()
{
    
  
    initChip(); // Initialize all the PORTs and do some configurations
    pwmMode(); 
   
    
    while(1) // Endless loop
    {      
         CCPR2L = counter; //set the duty cycle value                         
                    
    }// End of While loop
        
        
        
 }// End of Main function