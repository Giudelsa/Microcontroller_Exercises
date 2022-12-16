;*********************
; File Header
;*********************
    list p=18F25k50, r=hex, n=0
    #include <p18F25k50.inc>	; file to provide register definitions for the specific processor (a lot of equ)

; File registers
counter equ  0x01
d1 equ 0x02 ;used for the delay function
d2 equ 0x03 ;used for the delay function
d3 equ 0x05
start equ 0x04
;*********************
; Reset Vector
;*********************

    ORG     0x1000	; Reset Vector
			; When debugging:0x0000; when loading: 0x1000
    GOTO    START

;*********************
; Interrupt Vector
;*********************

    ORG     0x1008	; Interrupt Vector HIGH priority
    GOTO    inter_high	; When debugging:0x008; when loading: 0x1008
    ORG     0x1018	; Interrupt Vector LOW priority
    GOTO    inter_low	; When debugging:0x0008; when loading: 0x1018

;*********************
; Program Code Starts Here
;*********************

    ORG     0x1020	; When debugging:0x020; when loading: 0x1020

START
    movlw   0x80	; load value 0x80 in work register
    movwf   OSCTUNE		
    movlw   0x70	; load value 0x70 in work register
    movwf   OSCCON		
    movlw   0x10	; load value 0x10 to work register
    movwf   OSCCON2		
    clrf    LATA 	; Initialize PORTA by clearing output data latches
    movlw   0x00 	; Value used to initialize data direction
    movwf   TRISA 	; Set PORTA as output
    movlw   0x00 	; Configure A/D for digital inputs 0000 1111
    movwf   ANSELA	
    movlw   0x00	; Configure comparators for digital input
    movwf   CM1CON0
    clrf    LATB	; Initialize PORTB by clearing output data latches
    movlw   0x01	; Value used to initialize data direction
    movwf   TRISB	; Set RB6 as SDI
    clrf    LATC	; Initialize PORTC by clearing output data latches
    movlw   0x00	; Value used to initialize data direction
    movwf   TRISC	; Set RC7 as SDO

    bcf     UCON,3	; to be sure to disable USB module
    bsf     UCFG,3	; disable internal USB transceiver
    
  ;set up the SPI port
    movlw 0x20
    movwf SSP1CON1 ;enable configuration of SCK, SDO, SDI and SS; idle = low; SPI master mode; clock = TMR2 /2
    movlw 0xC0
    movwf SSP1STAT ; sample data in the middle; transmit active->idle
   
  ;configure the timer
    movlw 0xC0
    movwf INTCON
    movlw 0x0A
    movwf PIE1
    movlw 0xFF
    movwf PR2
    bcf PIE1,1
    
main
    clrf   counter ; clear registers
    clrf   d1
    clrf   d2
    movlw   0x01	; move 0x01 to Register counter
    movwf   counter
  
loop
    btfsc   PORTB,7   ; check the RB7 value if it 0
    goto    funcup      ; if RB7 is = 0 we go up
    goto    funcdown   ; if RB7 is = 1 we go down
    goto loop

funcup         	;Counter up
    incf  counter; we store the value on WREG
    movf  counter ;we save the incremented value in counter
    goto transmission
    
funcdown        ;Counter down
    decf  counter ; we store the value on WREG
    movf  counter ;we save the decremented value in counter
    goto transmission

    
transmission
    movff counter, SSP1BUF
    call delay1
    goto loop
    

; end of main loop
    
; your delay routine
delay1 ; complete this with e.g. something similar as a for loop delay ... but in assembly
    INCFSZ d1, 
    GOTO delay1
    INCFSZ d2
    goto delay1
    INCFSZ d3
    goto delay1
    
    return


; interrupt handling
inter_high
   
    
    btfsc SSP1STAT,0
    movff SSP1BUF, LATA
    bcf PIR1, 3
    RETFIE
    
inter_low
    
    nop
    RETFIE
        
    END