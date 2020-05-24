/******************************************************************************/
/*  Files to Include                                                          */
/******************************************************************************/
#ifdef __XC32
#include <xc.h>          /* Defines special funciton registers, CP0 regs  */
#endif

#include <stdint.h>         /* For uint32_t definition                        */
#include <stdbool.h>        /* For true/false definition                      */
#include <sys/appio.h>
#include <math.h>
//#include <string.h>
#include <stdio.h>
//#include "UART.h"
//#include "definitions.h"
//#include "delay.h"
//#include <dsplib_dsp.h>
//#include "dsp.h"
//#include "soft_i2c.h"


//#include <string.h>
//#include <stddef.h> // Defines NULL
//#include <stdlib.h>
////#include "proc/p32mz2048efh64.h"
//#include <sys/attribs.h>


// PIC32MZ2048EFH064 Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
// USERID = No Setting
#pragma config FMIIEN = OFF              // Ethernet RMII/MII Enable (MII Enabled)
#pragma config FETHIO = ON             // Ethernet I/O Pin Select (Default Ethernet I/O)
#pragma config PGL1WAY = OFF            // Permission Group Lock One Way Configuration (Allow multiple reconfigurations)
#pragma config PMDL1WAY = OFF           // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO = OFF           // USB USBID Selection (Controlled by Port Function)

#pragma config FPLLIDIV = DIV_1         // System PLL Input Divider (1x Divider)
#pragma config FPLLRNG = RANGE_8_16_MHZ // System PLL Input Range (5-10 MHz Input)
#pragma config FPLLICLK = PLL_POSC      // System PLL Input Clock Selection (POSC is input to the System PLL)
#pragma config FPLLMULT = MUL_50        // System PLL Multiplier (PLL Multiply by 40)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (2x Divider)
#pragma config UPLLFSEL = FREQ_12MHZ    // USB PLL Input Frequency Selection (USB PLL input is 12 MHz)


// DEVCFG1
#pragma config FNOSC = SPLL             // Oscillator Selection Bits (System PLL)
#pragma config DMTINTV = WIN_0     // DMT Count Window Interval (Window/Interval value is 15/16 counter value)
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disable SOSC)
#pragma config IESO = OFF               // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = EC             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Enabled, FSCM Enabled)
#pragma config WDTPS = PS16384             // Watchdog Timer Postscaler (1:32)
#pragma config WDTSPGM = STOP           // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
#pragma config WINDIS = NORMAL          // Watchdog Timer Window Mode (Watchdog Timer is in Window mode)
#pragma config FWDTEN = OFF              // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window size is 25%)
#pragma config DMTCNT = DMT10           // Deadman Timer Count Selection (2^10 (1024))
#pragma config FDMTEN = OFF             // Deadman Timer Enable (Deadman Timer is disabled)

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx2        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config TRCEN = OFF              // Trace Enable (Trace features in the CPU are disabled)
#pragma config BOOTISA = MIPS32         // Boot ISA Selection (Boot code and Exception code is MIPS32)
#pragma config FECCCON = OFF_UNLOCKED   // Dynamic Flash ECC Configuration (ECC and Dynamic ECC are disabled (ECCCON bits are writable))
#pragma config FSLEEP = OFF             // Flash Sleep Mode (Flash is powered down when the device is in Sleep mode)
#pragma config DBGPER = ALLOW_PG2       // Debug Mode CPU Access Permission (Allow CPU access to Permission Group 2 permission regions)
#pragma config SMCLR = MCLR_NORM        // Soft Master Clear Enable bit (MCLR pin generates a normal system Reset)
#pragma config SOSCGAIN = GAIN_0_5X     // Secondary Oscillator Gain Control bits (0.5x gain setting)
#pragma config SOSCBOOST = OFF          // Secondary Oscillator Boost Kick Start Enable bit (Normal start of the oscillator)
#pragma config POSCGAIN = GAIN_0_5X     // Primary Oscillator Gain Control bits (0.5x gain setting)
#pragma config POSCBOOST = OFF          // Primary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config EJTAGBEN = NORMAL        // EJTAG Boot (Normal EJTAG functionality)

// DEVCP0
#pragma config CP = OFF                 // Code Protect (Protection Disabled)
// SEQ3

// DEVADC0

// DEVADC1

// DEVADC2

// DEVADC3

// DEVADC4

// DEVADC7

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define FCY 150000000
#define FPB 150000000
#define BAUDRATE1 921600 //Main comm
//#define BAUDRATE2 115200 //
//#define BAUDRATE3 115200 //
//#define BAUDRATE4 38400  //
#define BAUDRATE5 9600 //
//#define BAUDRATE6 38400  //
//#define BRGVAL ((FCY/BAUDRATE1)/4)-1

volatile float samplinglength=10000.0;                              //Number of samples (maximum 128000)
volatile long timeforsample0=0,timeforsample1=0,timeforsample2=0;   //Timing holders for ADC sampling
volatile int mb=0,comdel=500,startdelay=3000;                       //mb counter for the PWM, comdel delay for uart buffers, startdelay delay after transmitted pulse.
volatile int16_t ramspace[128000];                                  //placeholder for sampled input
volatile int ADC0Result, threswhen=5000000;                         //ADC0Result not used, threswhen counter position when sampling threshild is reached.
volatile int thresh=2650,thres128=0;                                //threshold level, thres128 when in rotating buffer did threshold occur
volatile long calval=0;
char string [51];                                                   //buffer for uart send string
char gpsdata [500];
char rawenabled1=0;                                                 //rawdata out enabled or not
int found=0, GPSact=0,mmi=0,GpsFeed=0;                                        //threshold(echo) was found indicator
char timestring[6];
char timestringSet[6];
char datestring[6];
char datestringSet[6];
char locationstring[50];
char timeraw[6];
char dateraw[6];
char fix=0;
char cyclic=0;
double timeMultiplicator=0.6666666666667;
//music playing theme
int length = 55; // 55 is the number of notes
//twinkle twinkle little star
char notes[] = "ccggaag ffeeddc ggffeed ggffeed ccggaag ffeeddc "; // a space represents a rest
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 , 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1   };
int tempo = 100;
volatile int pulsess=0;
//end of music playing theme


void __attribute__ ((interrupt(IPL1SRS), vector(_EXTERNAL_4_VECTOR)))
PPS_GPS(void);
void __attribute__( (interrupt(ipl2), vector(_TIMER_1_VECTOR)))
Timer1Handler ( void );
void __attribute__( (interrupt(ipl3), vector(_ADC_VECTOR)))
ADC0Handler ( void );

void initInterrupts (void);
 void init_uarts(void)         
 {    


    
  int ON=15;          
  int PDSEL=1;          
  int STSEL=0;         
  int BRGH=3;         
  int UTXEN=10;         
  int URXEN=12; 
 
     U1BRG = FPB/(4.0*BAUDRATE1)-1;         
     U1MODE =         
         (1<<ON) |       // UART1 enabled         
         (1<<BRGH) |     // baudrate generation with equation 21-2         
         (0b00<<PDSEL) | // 8 databits, no parity (default)         
         (0<<STSEL);     // 1 stopbit (default)         
     U1STA =         
         (1<<UTXEN) |    // transmission enabled         
         (1<<URXEN);     // reception enabled           
    
//     U2BRG = FPB/(4.0*BAUDRATE2)-1;         
//     U2MODE =         
//         (1<<ON) |       // UART2 enabled         
//         (1<<BRGH) |     // baudrate generation with equation 21-2         
//         (0b00<<PDSEL) | // 8 databits, no parity (default)         
//         (0<<STSEL);     // 1 stopbit (default)         
//     U2STA =         
//         (1<<UTXEN) |    // transmission enabled         
//         (1<<URXEN);     // reception enabled   
//    
//     U3BRG = FPB/(4.0*BAUDRATE3)-1;         
//     U3MODE =         
//         (1<<ON) |       // UART3 enabled         
//         (1<<BRGH) |     // baudrate generation with equation 21-2         
//         (0b00<<PDSEL) | // 8 databits, no parity (default)         
//         (0<<STSEL);     // 1 stopbit (default)         
//     U3STA =         
//         (1<<UTXEN) |    // transmission enabled         
//         (1<<URXEN);     // reception enabled    
//    
//     U4BRG = FPB/(4.0*BAUDRATE4)-1;         
//     U4MODE =         
//         (1<<ON) |       // UART4 enabled         
//         (1<<BRGH) |     // baudrate generation with equation 21-2         
//         (0b00<<PDSEL) | // 8 databits, no parity (default)         
//         (0<<STSEL);     // 1 stopbit (default)         
//     U4STA =         
//         (1<<UTXEN) |    // transmission enabled         
//         (1<<URXEN);     // reception enabled    
//    
     U5BRG = FPB/(4.0*BAUDRATE5)-1;         
     U5MODE =         
         (1<<ON) |       // UART5 enabled         
         (1<<BRGH) |     // baudrate generation with equation 21-2         
         (0b00<<PDSEL) | // 8 databits, no parity (default)         
         (0<<STSEL);     // 1 stopbit (default)         
     U5STA =         
         (1<<UTXEN) |    // transmission enabled         
         (1<<URXEN);     // reception enabled    
    
//     U6BRG = FPB/(4.0*BAUDRATE6)-1;         
//     U6MODE =         
//         (1<<ON) |       // UART6 enabled         
//         (1<<BRGH) |     // baudrate generation with equation 21-2         
//         (0b00<<PDSEL) | // 8 databits, no parity (default)         
//         (0<<STSEL);     // 1 stopbit (default)         
//     U6STA =         
//         (1<<UTXEN) |    // transmission enabled         
//         (1<<URXEN);     // reception enabled    
 }   


void __attribute__ ((interrupt(IPL1SRS), vector(_EXTERNAL_4_VECTOR)))
PPS_GPS(void);
void PPS_GPS(void)
 {
calval=TMR6;
TMR6=0;
char ii =0;
do{
timestringSet[ii]=timestring[ii];
datestringSet[ii]=datestring[ii];
ii++;
}while(ii<6);
IEC0bits.T6IE=0; 
IFS0CLR = _IFS0_INT4IF_MASK;
 }

void write_i2c_start(void)
{
     /*  delaymicro(6); //Start i2c
       SDA_PIN = 0;
       delaymicro(6);
       SCL_PIN = 0;
       delaymicro(6);*/
}

void write_i2c_stop(void)
{
       /*SDA_PIN = 1;
       delaymicro(6);
       SCL_PIN = 1;
       delaymicro(6);*/
}

void write_i2c_ack(void)
{
      /* SCL_DIR = 0;
       SDA_DIR = 0;
       
       SDA_PIN = 0;
       delaymicro(6);
       SCL_PIN = 1;
       delaymicro(6);
       SCL_PIN = 0;
       delaymicro(6);*/
}

void write_i2c_Nack(void)
{
       /*SCL_DIR = 0;
       SDA_DIR = 0;
       
       SDA_PIN = 1;
       delaymicro(6);
       SCL_PIN = 1;
       delaymicro(6);
       SCL_PIN = 0;
       delaymicro(6);*/
}

char read_i2c(void){
   char outvar=0;
    /* SDA_DIR = 1;
    unsigned char bits[8];
    int i=0;
      
       char cnt=0;
       do{
      
       delaymicro(3);
       SCL_PIN = 1;
       delaymicro(3);
       bits[cnt]=SDA_PIN;
       delaymicro(3);
       SCL_PIN = 0;
       delaymicro(3);
       cnt++;
       }while(cnt<8);
       delaymicro(3);
       cnt=0;
//       char vals[8];
//       vals[0]=1;vals[1]=1;vals[2]=1;vals[3]=1;vals[3]=1;vals[0]=1;vals[0]=1;vals[0]=1;
       do{
           outvar+=bits[7-cnt]*pow(2,cnt);
           cnt++;
       }while (cnt<8);
      */
       return outvar;
}

void write_i2c(char invar){
   /*
    unsigned char mask = 1; // Bit mask
    unsigned char bits[8];
    int i=0;
    // Extract the bits
    for (i = 7; i >-1; i--) {
        // Mask each bit in the byte and store it
        bits[7-i] = (invar >> i) & mask;
    }
    // For debug purposes, lets print the received data

   
      
       char cnt=0;
       do{
       if(bits[cnt]==1){SDA_PIN = 1;}else{SDA_PIN = 0;}
       delaymicro(1);
       SCL_PIN = 1;
       delaymicro(5);
       SCL_PIN = 0;
       delaymicro(6);
       cnt++;
       }while(cnt<8);*/

}

//float checktemp(int board)
//{
//    float tempvar=0.0;
//    write_i2c_start();
//    if (board==1){write_i2c(144);}
//    if (board==2){write_i2c(146);}
//    write_i2c_ack();
//    write_i2c(0);
//    write_i2c_ack();
//    write_i2c_stop();
//    delaymicro(100);
//    write_i2c_start();
//    if (board==1){write_i2c(145);}
//    if (board==2){write_i2c(147);}
//    write_i2c_ack();
//    int8_t msb =read_i2c();
//    write_i2c_ack();
//    uint8_t lsb =read_i2c();
//    write_i2c_Nack();
//    write_i2c_stop();
//    Nop();
//    Nop();
//   
//    tempvar = msb + (lsb/128)*0.5;
//   
//    return tempvar;
//}

void __attribute__( (interrupt(ipl2), vector(_TIMER_1_VECTOR)))
Timer1Handler ( void );
void Timer1Handler(void)
{
   
    IFS0bits.T1IF = 0;
    TMR1=0;
    mb++;
    if(LATDbits.LATD0==1){
    LATD=2048;    
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    LATD=3072;
    }else{
    LATD=512;
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
    LATD=513; 
    }

   
    //if(mb>pulsess){T1CONbits.TON=0;} 

   
}

void __attribute__( (interrupt(ipl2), vector(_TIMER_6_VECTOR)))
Timer6Handler ( void );
void Timer6Handler(void)
{
    TMR6=0;
    IFS0bits.T6IF = 0;
    offlineTimeSet();
}


void __attribute__( (interrupt(ipl3), vector(_ADC_VECTOR)))
ADC0Handler ( void );
void ADC0Handler(void)
{
/* Verify if data for AN0 is ready. This bit is self cleared upon data read */

if(ADCDSTAT1bits.ARDY10)
{
ADC0Result = ADCDATA10;

}
   IFS2bits.ADCD10IF=0;
}

void initInterrupts(void)
{
asm volatile("di"); // This is the "KEY" to Disable "Off" All Interrupts
 asm volatile("ehb"); // This is the "KEY" to Disable "Off" All Interrupts
 

 INTCONbits.MVEC = 1; // Enable Multi-vectored Interrupts
 PRISS = 0x76543210;
   IFS0bits.T1IF = 0;
    IEC0bits.T1IE=1;
    IPC1bits.T1IP=1;
    IFS0bits.T6IF = 0;
    IEC0bits.T6IE=1;
    IPC7bits.T6IP=1;
    IFS2bits.ADCD10IF=0;
    IEC2bits.ADCD10IE= 1;
    INTCONbits.INT4EP = 1; //interrupt on rising edge
    IPC5bits.INT4IP=1;
    IPC5bits.INT4IS=1;
    IFS0bits.INT4IF=0;
    IEC0bits.INT4IE=1;
    IFS4bits.DMA0IF=0;
    IEC4bits.DMA0IE= 1;
  asm volatile("ei");

    __builtin_enable_interrupts();
   
//    setupDACDMA();
}

void delaypico(int nn)
{
int mm=0;
for(mm=0; mm<nn; mm++){}
}


void setvoltage (int volt)
{
    TRISEbits.TRISE7=1;
    LATEbits.LATE7=1;
    ANSELEbits.ANSE7=1;
    double resultet=0.0;
    double resultkeeper=0.0;
    long count=0;
    ADCCON1bits.ON = 0;
    ADCCSS1bits.CSS15 = 1; // AN15 (Class 3) set for scan
    ADCCSS1bits.CSS18 = 0; // AN18 (Class 3) remove from scan
    delaymilli(10);
    ADCCON1bits.ON = 1;
    int samples=10;
    ADCCON3bits.GSWTRG = 0;

    do{
    delaymillimicron(100);
    ADCCON3bits.GSWTRG = 1;
    while (ADCDSTAT1bits.ARDY15 == 0);
        /* fetch the result */
    resultet += ADCDATA15;
    count++;
    }while(count<samples);
    resultet=resultet/samples;
    resultkeeper=(85*(resultet/4096*3.3)-0.0299)-10;
    //printString("Result keeper before charging: "); printDouble(resultkeeper); printEL();
    //printString("Resultet before charging: "); printDouble(resultet); printEL();
        if (volt>resultkeeper){
            Nop();
            Nop();
                resultet=0;
                do{
                resultet=0;
                count++;
                int voltinc=0;
                do{
                LATDbits.LATD4=1;
                delaymillimicron(400);
                LATDbits.LATD4=0;
                delaymillimicron(200);
                voltinc++;
                }while(voltinc<30);
                delaymillimicron(10000);
                int ycount=0;
                do{
                delaymillimicron(100);
                ADCCON3bits.GSWTRG = 1;
                while (ADCDSTAT1bits.ARDY15 == 0);
                    /* fetch the result */
                resultet += ADCDATA15;
                ycount++;
                }while(ycount<samples);
                resultet=resultet/samples;
                resultkeeper=(85*(resultet/4096*3.3)-0.0299)-10;
                if(resultkeeper<0){resultkeeper=0;}
               
                }while(resultkeeper<volt && count<100000);
    }

   
        if (volt<resultkeeper+5){
            LATFbits.LATF5=0;
                resultet=0;
                int ycount=0;
                do{
                resultet=0;
                ycount++;
                delaymillimicron(100000);
                count=0;
                do{
                delaymillimicron(100);
                ADCCON3bits.GSWTRG = 1;
                while (ADCDSTAT1bits.ARDY15 == 0);
                    /* fetch the result */
                resultet += ADCDATA15;
                count++;
                }while(count<samples);
                resultet=resultet/samples;
                resultkeeper=(85*(resultet/4096*3.3)-0.0299)-10;
               
                }while(resultkeeper>volt && ycount<20000);

                LATFbits.LATF5=1;
                Nop();
                Nop();
        }   
         ADCCON1bits.ON = 0;
//         TRISEbits.TRISE7=0;
//         LATEbits.LATE7=0;
         ANSELEbits.ANSE7=0;
        //printString("Result keeper after charging: ");printDouble(resultkeeper);printEL();
}

void setIOforSensing (void)
{
Nop();
Nop();
Nop();
LATEbits.LATE1=1; //Enable 1
LATEbits.LATE2=1; //Enable 2
LATBbits.LATB13=0; //input enable
LATBbits.LATB14=0; //input enable
ANSELEbits.ANSE4=1;
TRISEbits.TRISE4=1;
Nop();
Nop();
Nop();
}
 
void setIOforTransmit (void)
{
LATEbits.LATE1=1; //Enable 1
LATEbits.LATE2=1; //Enable 2    
LATBbits.LATB13=1;
LATBbits.LATB14=1;
LATDbits.LATD0=1;    
LATDbits.LATD9=1;
LATDbits.LATD11=0;
LATDbits.LATD10=0;
ANSELEbits.ANSE4=0;
TRISEbits.TRISE4=0;
LATEbits.LATE4=0;
 }
 
 int PPStoCrystalCal(void)
 {
     long calibval=0,returnval=0,timeout=0;
     int ii=0;
 
    do{
       timeout=0; 
       do{timeout++;}while(TMR6>60 && timeout<160000000); 
       calibval+=calval;
       ii++;
     }while(ii<10 && timeout<159999999);
     calibval=calibval/10;
     if(calibval<200000000 && calibval>100000000 && timeout<159999999){
     returnval=calibval;   
     PR6=2*calibval;
     timeMultiplicator=100000000.0/calibval;
     Nop();
     }else{       
//     timeMultiplicator=0.66666666666667;   
//     PR6=300000000;
     returnval=timeout;
     }
   return returnval; 
 }
 
 void setup_ports(void){
  TRISDbits.TRISD1=1;       //TRG

  TRISCbits.TRISC14=0;      //TRCD1 HIGH OUT
  TRISCbits.TRISC13=0;      //TRCD1 LOW OUT 
  TRISDbits.TRISD0=0;       //TRCD upper bridge enable
  TRISDbits.TRISD2=1;       //UART receive
  TRISDbits.TRISD3=0;       //UART transmit
  TRISDbits.TRISD10=0;      //TRCD lower bridge enable 
  TRISDbits.TRISD11=0;     //TRCD lower bridge
  TRISFbits.TRISF5=0;      //Lower voltage output, active low!  
  TRISDbits.TRISD9=0;      //TRCD upper bridge  
  TRISDbits.TRISD5=0;      //High voltage lower transistor  
 
  TRISEbits.TRISE7=1;       //AD voltage measure high voltage in
  TRISBbits.TRISB0=1;       //TRDC2 INPUT
  TRISBbits.TRISB2=1;       //TRDC2 INPUT
  TRISBbits.TRISB3=0;       //TRDC1 INPUT  
  TRISBbits.TRISB13=0;      //INPUT amplifier disable
  TRISBbits.TRISB14=0;      //INPUT amplifier disable
  LATBbits.LATB13=1;        //Disabled
  LATBbits.LATB14=1;        //Disabled
  TRISDbits.TRISD3=0;       //300V CHARGE DONE
  TRISDbits.TRISD4=0;       //300V CHARGE ENABLE
  TRISBbits.TRISB15=1;
  LATDbits.LATD0=1;
  LATDbits.LATD5=0;         //Turn off HV lowering transistor (high = HV enabled)
  LATDbits.LATD10=1;
  LATDbits.LATD11=0;
  LATCbits.LATC13=0;
  LATCbits.LATC14=0;
  LATEbits.LATE5=0;
  LATEbits.LATE6=0;
  LATGbits.LATG6=0;
  LATBbits.LATB13=0;        //Turn off AD-input diable (High = AD disabled)
  TRISEbits.TRISE1=0;
  TRISEbits.TRISE2=0;
  TRISEbits.TRISE4=1;
  TRISEbits.TRISE5=1;   //Alternative GPS PPS input
  TRISBbits.TRISB6=0;       //transducer disable input
  TRISBbits.TRISB7=0;       //transducer disable input
  TRISBbits.TRISB9=1; //GPS UART input
  TRISBbits.TRISB10=1; //GPS PPS input
  TRISBbits.TRISB11=1; //GPS PPS input
  TRISFbits.TRISF0=0; //GPS UART output //change to RB10 in later design.
 
  LATBbits.LATB6=1;
  LATBbits.LATB7=1;
  LATDbits.LATD9=0;
  LATDbits.LATD4=0;
  LATFbits.LATF5=1; //Lower voltage off
 
  /* initialize ADC calibration setting */
  ANSELB=0x00000000; ANSELE=0x00000000; ANSELG=0x00000000;
 
  ANSELEbits.ANSE7=1;
  ANSELEbits.ANSE4=1;
  ANSELBbits.ANSB0=1;
 
  //CNENBbits.CNIEB10=1;
  //CNENBbits.CNIEB11=1;
  //CNENEbits.CNIEE5=1;
  //CNCONCSET = (1 << 15);
LATEbits.LATE1=1; //Enable 1
LATEbits.LATE2=1; //Enable 2    
/* Configure ADCCON1 */
ADCCON1 = 0; // No ADCCON1 features are enabled including: Stop-in-Idle, turbo,
// CVD mode, Fractional mode and scan trigger source.
ADCCON1bits.SELRES = 3; // ADC7 resolution is 12 bits
ADCCON1bits.STRGSRC = 1; // Select scan trigger.

/* Configure ADCCON2 */
ADCCON2bits.SAMC = 0; // ADC7 sampling time = 5 * TAD7
ADCCON2bits.ADCDIV = 0; // ADC7 clock freq is half of control clock = TAD7

/* Initialize warm up time register */
ADCANCON = 0;
ADCANCONbits.WKUPCLKCNT = 1; // Wakeup exponent = 32 * TADx
/* Clock setting */
ADCCON3bits.ADCSEL = 1; // Select input clock source
ADCCON3bits.CONCLKDIV = 0; // Control clock frequency is half of input clock
ADCCON3bits.VREFSEL = 1; // Select AVdd and AVss as reference source
ADC0TIMEbits.ADCDIV = 0; // ADC0 clock frequency is half of control clock = TAD0
ADC0TIMEbits.ADCEIS = 0; // ADC0 clock frequency is half of control clock = TAD0
ADC0TIMEbits.SAMC = 0; // ADC0 sampling time = 5 * TAD0
ADC0TIMEbits.SELRES = 3; // ADC0 resolution is 12 bits
/* Select analog input for ADC modules, no presync trigger, not sync sampling */
ADCTRGMODEbits.SH0ALT = 0; // ADC0 = AN0
/* Select ADC input mode */

ADCIMCON1bits.SIGN0 = 0; // unsigned data format
ADCIMCON1bits.DIFF0 = 0; // Single ended mode

/* Configure ADCGIRQENx */
ADCGIRQEN1 = 0; // No interrupts are used.
ADCGIRQEN2 = 0;
//ADCGIRQEN1bits.AGIEN10=1;
/* Configure ADCCSSx */
ADCCSS1 = 0; // Clear all bits
ADCCSS1bits.CSS15 = 1; // AN15 (Class 3) set for scan
ADCCSS1bits.CSS18 = 1; // AN18 (Class 3) set for scan

/* Configure ADCCMPCONx */
ADCCMPCON1 = 0; // No digital comparators are used. Setting the ADCCMPCONx
ADCCMPCON2 = 0; // register to '0' ensures that the comparator is disabled.
ADCCMPCON3 = 0; // Other registers are ?don't care?.
ADCCMPCON4 = 0;
ADCCMPCON5 = 0;
ADCCMPCON6 = 0;
/* Configure ADCFLTRx */
ADCFLTR1 = 0; // No oversampling filters are used.
ADCFLTR2 = 0;
ADCFLTR3 = 0;
ADCFLTR4 = 0;
ADCFLTR5 = 0;
ADCFLTR6 = 0;
/* Early interrupt */
ADCEIEN1 = 0; // No early interrupt
ADCEIEN2 = 0;
/* Turn the ADC on */
ADCCON1bits.ON = 1;
/* Wait for voltage reference to be stable */
while(!ADCCON2bits.BGVRRDY); // Wait until the reference voltage is ready
while(ADCCON2bits.REFFLT); // Wait if there is a fault with the reference voltage
/* Enable clock to analog circuit */
ADCANCONbits.ANEN0 = 1; // Enable the clock to analog bias ADC0
ADCANCONbits.ANEN7 = 1; // Enable, ADC7

/* Wait for ADC to be ready */
while(!ADCANCONbits.WKRDY0); // Wait until ADC0 is ready
while(!ADCANCONbits.WKRDY7); // Wait until ADC7 is ready
/* Enable the ADC module */
ADCCON3bits.DIGEN0 = 1; // Enable ADC0
ADCCON3bits.DIGEN7 = 1; // Enable ADC7


T1CON = 0x0; // Stop any 16-bit Timer5 operation
T1CONSET = 0x0000; // Enable 32-bit mode, prescaler 1:1,
 // internal peripheral clock source
TMR1 = 0x0; // Clear contents of the TMR4 and TMR5
T1CONbits.TON = 0; // Stop Timer1

T2CON = 0x0; // Stop any 16-bit Timer5 operation
T2CONSET = 0x0000; // Enable 32-bit mode, prescaler 1:1,
 // internal peripheral clock source
TMR2 = 0x0; // Clear contents of the TMR4 and TMR5
T2CONbits.TON = 0; // Stop Timer1

T4CON = 0x0; // Stop any 16/32-bit Timer4 operation
T5CON = 0x0; // Stop any 16-bit Timer5 operation
T4CONSET = 0x0008; // Enable 32-bit mode, prescaler 1:1,
T6CON = 0x0; // Stop any 16/32-bit Timer4 operation
T7CON = 0x0; // Stop any 16-bit Timer5 operation
T6CONSET = 0x0008; // Enable 32-bit mode, prescaler 1:1,
 // internal peripheral clock source
TMR4 = 0x0; // Clear contents of the TMR4 and TMR5
TMR6 = 0x0; // Clear contents of the TMR6 and TMR7
PR4 = 0xFFFFFFFF; // Load PR4 and PR5 registers with 32-bit value
T4CONSET = 0x8000; // Start Timer4/5
PR6 = 0xFFFFFFFF; // Load PR4 and PR5 registers with 32-bit value
T6CONSET = 0x8000; // Start Timer4/5
}
 
 void setup_pps(void)        
 {        
     SYSKEY = 0x0;        
     SYSKEY = 0xAA996655;        
     SYSKEY = 0x556699AA;        
     PB4DIV = 0x00008000;
     CFGCONbits.IOLOCK = 0;        
    
     INT4Rbits.INT4R = 0b0110; // Set INT4 to RPE3 
    
     U1RXRbits.U1RXR = 0b0000; // Set RX1 to RPD2        
     RPD3Rbits.RPD3R = 0b0001; // Set TX1 to RPD3        
    
     U5RXRbits.U5RXR = 0b0101; // Set RX1 to RPB9        
     RPF0Rbits.RPF0R = 0b0011; // Set TX1 to RPF0       Has to change to RB11 later!
    
     CFGCONbits.IOLOCK = 1;        
 
     SYSKEY = 0x33333333;        
 } 
 
void Nreadinput(void)
{

    ADCCON1bits.ON = 1;
    int valrec=0,threshit=0;
    long counter=0;
    threswhen=5000000;
    memset(ramspace,0,sizeof(ramspace));
    thres128=0;
   
    ADCCSS1bits.CSS15 = 0; //AN15 (Class 3) quit scanning
    ADCCSS1bits.CSS18 = 1; //AN18 (Class 3) set for scan
    ADCCON3bits.GSWTRG = 1;

    timeforsample0=TMR4;   //take time before first sample
    ramspace[valrec]=2500;
    int dummy = ADCDATA18;
    dummy = ADCDATA18;
    found=0;
    ADCCON3bits.GSWTRG = 1;
    do{
    /* fetch the result */
        while (ADCDSTAT1bits.ARDY18 == 0);
        ramspace[valrec] = ADCDATA18;
        if(ramspace[valrec]>thresh && threshit == 0 && counter>1000){
            timeforsample1=TMR4;
            threshit=1;
            threswhen=counter;
            thres128=valrec;
            found=1;
        }  //Threshold was hit, store time to threshold
        valrec++;
        counter++;
        if (valrec>=128000){
            valrec=0;
        }
        ADCCON3bits.GSWTRG = 1;
    }while(counter<5000000 && counter < (threswhen+samplinglength+1));              //wait up to about 1 second for an echo else quit
    timeforsample2=TMR4;                                                            //store the time to end
    T4CONbits.TON = 0;
    int ikl=0;
   
    if (thres128<1000){
    for (ikl = thres128+samplinglength; ikl > thres128; ikl--) {
        ramspace[ikl]=ramspace[ikl-thres128];                                       //make place for and store history values from temparr to ramspace
    }
    for (ikl = 127999; ikl > 127999-thres128; ikl--) {
        ramspace[127999-ikl]=ramspace[ikl];                                         //make place for and store history values from temparr to ramspace
    }
    thres128=1000;
    }else{
       
    }
    // FIRfilterdata(); //Needs work
   
    ADCCON3bits.VREFSEL=0;
}   
   
//void FIRfilterdata (void){
//int i;
//int K = 8;
//int N = 100;
//int16 coeffs[37]={73,100,67,-65,-323,-710,-1192,-1696,-2119,-2343,-2265,-1814,-985,160,1490,2823,3963,4730,5000,4730,3963,2823,1490,
//160,-985,-1814,-2265,-2343,-2119,-1696,-1192,-710,-323,-65,67,100,73};
//int16 coeffs2x[2*K];
//int16 delayline[K];
//int16 indata[N];
//int16 outdata[N];
//
//
//for (i = 0; i < K; i++)
//delayline[i] = 0;
//// load coefficients into coeffs here
//
//int y;
//i=0,y=0;
//mips_fir16_setup(coeffs2x, coeffs, K);
//while (i<10000)
//{
//    for (y = 0; y < 100; y++) {
//    indata[y] = ramspace[i+y];
//    }  
//
//mips_fir16(outdata, indata, coeffs2x,
//delayline, N, K, 3);
//    for (y = 0; y < 100; y++) {
//    ramspace[y+i] = outdata[y];
//    }   
//i+=100;
//}
//}

void setupDACDMA (void)
{
    ADCCON1bits.ON = 0;
    ADCGIRQEN1bits.AGIEN10 = 1; // Enable data ready interrupt for AN0
    ADCCON1bits.ON = 1; // turn ADC module on
}
 
void setfreq(float frequency)
{

int rerrr=0;
rerrr=((150000000/frequency)/2)-27;
PR1=rerrr;
Nop();
}

void playTone(int tone, int duration) {
    long i=0;
    for (i = 0; i < duration * 1000L; i += tone * 2) {
    LATDINV = 0b0000101000000000;  
    delaymillimicron(tone*2);
    LATDINV = 0b0000101000000000;  
    delaymillimicron(tone*2);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  int i=0;
  // play the tone corresponding to the note name
  for (i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void tinkle() {
LATDbits.LATD11=1;
LATDbits.LATD9=0; 
int i=0;
  for (i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delaymilli(beats[i] * tempo); // rest
    } else {
        setvoltage(150);
      playNote(notes[i], beats[i] * tempo);
    }
   
    // pause between notes
    delaymilli(tempo / 2);
  }
}

void sendUartCharArray()
{
    unsigned int iyu=0;int ugu=0;
    char bulle=0;
    iyu=0;
    do{
    ugu=string[iyu];
    bulle=ugu;
    while (U1STAbits.UTXBF);
    U1TXREG = bulle;
    delaymicro(1);
    iyu++;
    }while(string[iyu]!=0x00);

}

void SPWMmodepulsetrain (int frequency, int numberofpulses)
{
   
    setIOforTransmit();
    LATDbits.LATD11=1; //1LoIn
    LATDbits.LATD9=1; //2LoIn
    LATDbits.LATD0=0; //1HiIn
    LATDbits.LATD10=0; //2HiIn
       
    unsigned char pwmv[64];    //storage for calculated positive sine vectors
    unsigned char pwmvvn[64];   //storage for calculated negative sine vectors
    unsigned int resolution=158; //pwm resolution
   
    int numofvals=18; //number of values for sine about 47kHz
    //Build prototype for converting frequency to numofvals here
    //
    //**********************************************************   
   
    int i=0;
    memset(pwmv,0,sizeof(pwmv));
    memset(pwmvvn,0,sizeof(pwmvvn));
   
    for(i = 0; i < numofvals; i++ ){
    pwmv[i] = 0.5*resolution + 0.5*resolution*sin(2*3.14159265359*i/(numofvals));
    pwmvvn[i]=resolution-pwmv[i];
    }

    int ii=0, ij=0, a=0;
    T6CONbits.TON = 1;
    do{
    do{ 
     LATD=513;
//     LATDbits.LATD11=0;  
//     Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
//     Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
//     Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
//     LATDbits.LATD0=1;
    
     while(TMR6 < pwmv[ij]);
     TMR6=0;
     LATD=2560;
//     LATDbits.LATD0=0;
//     Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
//     Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
//     Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();
//     LATDbits.LATD11=1; 
     while(TMR6 < pwmvvn[ij]);
     TMR6=0;
     ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    //delaymilli(1);
LATDbits.LATD11=1; //1LoIn
LATDbits.LATD9=1; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
if(startdelay>110){delaymicro(startdelay-50);}else{delaymicro(150);} //initial delay before activating listening inputs
delaymilli(8);
LATDbits.LATD11=0; //1LoIn
LATDbits.LATD9=0; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
setIOforSensing();

    Nop();
    Nop();
   
}

void SPWMmodechirp (int frequency, int numberofpulses)
{
   
    setIOforTransmit();
    LATDbits.LATD11=1; //1LoIn
    LATDbits.LATD9=1; //2LoIn
    LATDbits.LATD0=0; //1HiIn
    LATDbits.LATD10=0; //2HiIn
       
    unsigned char pwmv1[64],pwmv2[64],pwmv3[64],pwmv4[64],pwmv5[64],pwmv6[64],pwmv7[64],pwmv8[64];    //storage for calculated positive sine vectors
    unsigned char pwmvvn1[64],pwmvvn2[64],pwmvvn3[64],pwmvvn4[64],pwmvvn5[64],pwmvvn6[64],pwmvvn7[64],pwmvvn8[64];   //storage for calculated negative sine vectors
    unsigned int resolution1=152,resolution2=154,resolution3=156,resolution4=158,resolution5=160,resolution6=162,resolution7=164,resolution8=166; //pwm resolution
   
    int numofvals=18; //number of values for sine about 47kHz
    //Build prototype for converting frequency to numofvals here
    //
    //**********************************************************   
   
    int i=0;
    memset(pwmv1,0,sizeof(pwmv1));
    memset(pwmvvn1,0,sizeof(pwmvvn1));
    memset(pwmv2,0,sizeof(pwmv2));
    memset(pwmvvn2,0,sizeof(pwmvvn2));
    memset(pwmv3,0,sizeof(pwmv3));
    memset(pwmvvn3,0,sizeof(pwmvvn3));
    memset(pwmv4,0,sizeof(pwmv4));
    memset(pwmvvn4,0,sizeof(pwmvvn4));
    memset(pwmv5,0,sizeof(pwmv5));
    memset(pwmvvn5,0,sizeof(pwmvvn5));
    memset(pwmv6,0,sizeof(pwmv6));
    memset(pwmvvn6,0,sizeof(pwmvvn6));
    memset(pwmv7,0,sizeof(pwmv7));
    memset(pwmvvn7,0,sizeof(pwmvvn7));
    memset(pwmv8,0,sizeof(pwmv8));
    memset(pwmvvn8,0,sizeof(pwmvvn8));    
   
    for(i = 0; i < numofvals; i++ ){
    pwmv1[i] = 0.5*resolution1 + 0.5*resolution1*sin(2*3.14159265359*i/(numofvals));
    pwmvvn1[i]=resolution1-pwmv1[i];
    }
   
        for(i = 0; i < numofvals; i++ ){
    pwmv2[i] = 0.5*resolution2 + 0.5*resolution2*sin(2*3.14159265359*i/(numofvals));
    pwmvvn2[i]=resolution2-pwmv2[i];
    }
   
        for(i = 0; i < numofvals; i++ ){
    pwmv3[i] = 0.5*resolution3 + 0.5*resolution3*sin(2*3.14159265359*i/(numofvals));
    pwmvvn3[i]=resolution3-pwmv3[i];
    }
   
        for(i = 0; i < numofvals; i++ ){
    pwmv4[i] = 0.5*resolution4 + 0.5*resolution4*sin(2*3.14159265359*i/(numofvals));
    pwmvvn4[i]=resolution4-pwmv4[i];
    }
   
        for(i = 0; i < numofvals; i++ ){
    pwmv5[i] = 0.5*resolution5 + 0.5*resolution5*sin(2*3.14159265359*i/(numofvals));
    pwmvvn5[i]=resolution5-pwmv5[i];
    }
   
        for(i = 0; i < numofvals; i++ ){
    pwmv6[i] = 0.5*resolution6 + 0.5*resolution6*sin(2*3.14159265359*i/(numofvals));
    pwmvvn6[i]=resolution6-pwmv6[i];
    }
   
        for(i = 0; i < numofvals; i++ ){
    pwmv7[i] = 0.5*resolution7 + 0.5*resolution7*sin(2*3.14159265359*i/(numofvals));
    pwmvvn7[i]=resolution7-pwmv7[i];
    }
   
        for(i = 0; i < numofvals; i++ ){
    pwmv8[i] = 0.5*resolution8 + 0.5*resolution8*sin(2*3.14159265359*i/(numofvals));
    pwmvvn8[i]=resolution8-pwmv8[i];
    }
   

    int ii=0, ij=0, a=0;
    T6CONbits.TON = 1;
    do{
    do{ 
     LATD=513;while(TMR6 < pwmv1[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn1[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;
   
        do{
    do{ 
     LATD=513;while(TMR6 < pwmv2[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn2[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;
           
        do{
    do{ 
     LATD=513;while(TMR6 < pwmv3[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn3[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;
           
    do{
    do{ 
     LATD=513;while(TMR6 < pwmv4[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn4[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;

    do{
    do{ 
     LATD=513;while(TMR6 < pwmv5[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn5[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;

    do{
    do{ 
     LATD=513;while(TMR6 < pwmv6[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn6[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;

    do{
    do{ 
     LATD=513;while(TMR6 < pwmv7[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn7[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;

    do{
    do{ 
     LATD=513;while(TMR6 < pwmv8[ij]);
     TMR6=0;LATD=2560;while(TMR6 < pwmvvn8[ij]);
     TMR6=0;ij++;
    }while(ij<numofvals);
    ij=0;
    ii++;
    }while(ii<numberofpulses);
    ii=00;
    numberofpulses=0;           
           
   
    //delaymilli(1);
LATDbits.LATD11=1; //1LoIn
LATDbits.LATD9=1; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
if(startdelay>110){delaymicro(startdelay-50);}else{delaymicro(150);} //initial delay before activating listening inputs
delaymilli(8);
LATDbits.LATD11=0; //1LoIn
LATDbits.LATD9=0; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
setIOforSensing();

    Nop();
    Nop();
   
}

void Ntransmitdata(void){
   
    unsigned int valueAD=0,count=1;
 
    count=thres128-1000;
   
    volatile double samplingtmbfr=0.0;
    volatile double timeval=0.0;
    volatile double tt1=0.0;
    if (threswhen!=0.0){samplingtmbfr=((timeforsample1-timeforsample0)/threswhen);}else{threswhen=1.0;}
    if(found==1){ //echo was found
    while (U1STAbits.UTXBF);
    tt1=(timeforsample1-((samplingtmbfr)*1000.0))/150000000.0;
    do{
    int cnt=(count-thres128+1000);
    timeval=tt1+(((timeforsample2-timeforsample1)/samplinglength)/150000000.0)*cnt;
    memset(string,0,sizeof(string));
    snprintf(string, 21, "%0.9f", timeval);
    sendUartCharArray(string); //send out the result on the comport
    while (U1STAbits.UTXBF);
    U1TXREG = ';';
    delaymicro(1);
       
    valueAD=ramspace[count];
    memset(string,0,sizeof(string));
    itoa(string,valueAD,10);
    sendUartCharArray(string); //send out the result on the comport
    while (U1STAbits.UTXBF);
    U1TXREG = ';';
    delaymicro(1);
   
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0D;
    count++;
    delaymicro(comdel); //delay set by command
    }while(count<thres128+samplinglength-999);
    Nop();
    }else{//echo was not seen
       
    memset(string,0,sizeof(string));
    strcat(string, "No signal received    ");
    sendUartCharArray(string); //send out the result on the comport
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0A;
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0D;

    }
}

void transmittimes(void){
    unsigned int it=0;
    T4CONbits.TON = 0;
    it=timeforsample1;
    memset(string,0,sizeof(string));
    itoa(string,it,10);
    sendUartCharArray(string);
   
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0A;
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0D;
   
    it=timeforsample2;
    memset(string,0,sizeof(string));
    itoa(string,it,10);
    sendUartCharArray(string);
   
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0A;
    while (U1STAbits.UTXBF);
    U1TXREG = 0x0D;
}

void send_pulsetrain(int pulses, int frequency, int voltage)
{
    T4CONbits.TON = 0;
    T1CONbits.TON = 0;
if (voltage>350){voltage=350;}              //limit the voltage
if (frequency>600000){frequency=600000;}    //limit the frequency
if (pulses>1000){pulses=1000;}              //limit the number of pulses
setvoltage(voltage); //max 640V p-p!!!
setfreq(frequency);   
pulsess=pulses;
mb=0;                                       //zero the counter

setIOforTransmit();

TMR4=0;                                     //Zero 32-bit timer
TMR1=0;                                     //Zero 16-bit timer 
T1CONbits.TON=1;                            //Activate timer for pulsetrain, interrupts are used
T4CONbits.TON = 1;                          //activate 32-bit 150MHz timer
while (mb<=(pulsess+3)*2);                          //mb increased when interrupt timer is fired until number of pulses is reached
T1CONbits.ON=0;                             //stop pulse out timer
mb=0;
LATDbits.LATD11=1; //1LoIn
LATDbits.LATD9=1; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
if(startdelay>110){delaymicro(startdelay-50);}else{delaymicro(150);} //initial delay before activating listening inputs
delaymicro(4000); 
LATDbits.LATD11=0; //1LoIn
LATDbits.LATD9=0; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
setIOforSensing();
delaymicro(1000);    

Nop();
Nop();
//Nreadinput();

Nop();
Nop();
if(rawenabled1==1){                                                     //transmit raw, if enabled
    Ntransmitdata();
}
}

void send_chirp_pulsetrain(int pulses, int voltage, int fromfreq, int tofreq)
{
if (voltage>350){voltage=350;}                  //limit the voltage
if (tofreq>500000){tofreq=500000;}              //limit the frequency
if (pulses>1000){pulses=1000;}                  //limit the number of pulses
setfreq(fromfreq);  
setvoltage(voltage);                            //max 640V p-p in current design!
mb=0;                                           //zero the counter
int PRRR=0;                                     //Pulse period range var
setIOforTransmit();

setIOforTransmit();
TMR4=0;                                     //Zero 32-bit timer
TMR1=0;                                     //Zero 16-bit timer 
T1CONbits.TON=1;                            //Activate timer for pulsetrain, interrupts are used
T4CONbits.TON = 1;                          //activate 32-bit 150MHz timer                                //activate 16-bit 150MHz timer 
do{                                             //frequency vary
    if(mb!=0){PRRR=fromfreq+(((tofreq-fromfreq)/pulses)*mb);}else{PRRR=fromfreq;}
    setfreq(PRRR);
}while (mb<pulses);                             //mb increased when interrupt timer is fired until number of pulses is reached
T1CONbits.TON=0; 
LATDbits.LATD11=1; //1LoIn
LATDbits.LATD9=1; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
if(startdelay>110){delaymicro(startdelay-50);}else{delaymicro(150);} //initial delay before activating listening inputs

LATDbits.LATD11=0; //1LoIn
LATDbits.LATD9=0; //2LoIn
LATDbits.LATD0=0; //1HiIn
LATDbits.LATD10=0; //2HiIn
setIOforSensing();
//delaymicro(1000);    

Nop();
Nop();
Nreadinput();

Nop();
Nop();
if(rawenabled1==1){                                                     //transmit raw, if enabled
    Ntransmitdata();
}
}

void setthreshold(int thres)
{
    thresh=thres;
}

void setstartdelay(int startd)
{
    if (startd>501){
    startdelay=startd;
    }
}

void setcomdelay(int coom)
{
    comdel=coom;

}

void activateTimer6(void)
{
        TMR6 = 0x0;
        T6CONbits.TON=1;
}

void inactivateTimer6(void)
{
        TMR6 = 0x0;
        T6CONbits.TON=0;
}

volatile void offlineTimeSet(void){
    char hourss,minutess,secondss,yearss,monthss,dayss;
    hourss=(timestring[0]-0x30)*10;
    hourss=hourss+(timestring[1]-0x30);
    minutess=(timestring[2]-0x30)*10;
    minutess=minutess+(timestring[3]-0x30);
    secondss=(timestring[4]-0x30)*10;
    secondss=secondss+(timestring[5]-0x30);

    secondss+=2;

    if(secondss>=60){
        secondss=secondss-60;
        minutess++;
            if(minutess==60){
            minutess=0;
            hourss++;
                if(hourss==24){
                hourss=0;
                }
            }
    }   
    timestring[0]=hourss/10;
    timestring[1]=hourss%10;
    timestring[2]=minutess/10;
    timestring[3]=minutess%10;
    timestring[4]=secondss/10;
    timestring[5]=secondss%10;
   
   
}

void receivecommand(void)
{
    if(U1STAbits.URXDA!=0){
    int timeout=0;  
    char addr[3];
    char dd=0;
    volatile char datat[40];
    int iic=0;
    char inchar=U1RXREG;
        timeout=0;
        if (inchar=='$'){
        timeout=0;do{timeout++;}while(U1STAbits.URXDA==0 && timeout<10000);
        addr[0]=U1RXREG-48;
        timeout=0;do{timeout++;}while(U1STAbits.URXDA==0 && timeout<10000);
        addr[1]=U1RXREG-48;
        timeout=0;do{timeout++;}while(U1STAbits.URXDA==0 && timeout<10000);
        addr[2]=U1RXREG-48;
        timeout=0;do{timeout++;}while(U1STAbits.URXDA==0 && timeout<10000);
        inchar=U1RXREG; //receive the last '$'
        if(addr[0]*100+addr[1]*10+addr[2]*1<999 && addr[0]*100+addr[1]*10+addr[2]*1>=0){
            memset(datat,0,sizeof(datat)); //clear string
            do{
           timeout=0;do{timeout++;}while(U1STAbits.URXDA==0 && timeout<1000);
           datat[iic]=U1RXREG;
           iic++;
           }while(iic<35 && timeout<998 && datat[iic]!='$');
          
           if(iic<30){
              
               if(datat[0]=='A' && datat[1]=='D'){
                   char aa,bb,cc;
                   aa=datat[2]-48;
                   bb=datat[3]-48;
                   cc=datat[4]-48;
                   if(aa*100+bb*10+cc*1 < 999 && aa*100+bb*10+cc*1 >= 0){
                   //setaddress(aa, bb, cc);
                   }
                  
               }
              
               if(datat[0]=='S' && datat[1]=='D'){
                char substr1[strlen(datat)-2];
                strncpy(substr1, &datat[2],strlen(datat)-1);
                int comdelli=0;
                sscanf(substr1, "%d", &comdelli);
                setcomdelay(comdelli);
               
                memset(string,0,sizeof(string));
                strcat(string, "UART delay set to ");
                char newstr[5];
                sprintf(newstr, "%d", comdelli);
                strcat(string, newstr);
                strcat(string, " microseconds");
               
                sendUartCharArray(string);
                while (U1STAbits.UTXBF);
                U1TXREG = 0x0A;
                while (U1STAbits.UTXBF);
                U1TXREG = 0x0D;
                goto label1;
               }
              
               if(datat[0]=='S' && datat[1]=='T'){
                char substr1[strlen(datat)-2];
                strncpy(substr1, &datat[2],strlen(datat)-1);
                int startdel;
                sscanf(substr1, "%d", &startdel);
                setstartdelay(startdel);
               
                memset(string,0,sizeof(string));
                strcat(string, "Sampling delay ");
                char newstr[5];
                sprintf(newstr, "%d", startdel);
                strcat(string, newstr);
                strcat(string, " microseconds");
               
                sendUartCharArray(string);
                while (U1STAbits.UTXBF);
                U1TXREG = 0x0A;
                while (U1STAbits.UTXBF);
                U1TXREG = 0x0D;
               
                goto label1;
               }
              
               if(datat[0]=='A' && datat[1]=='R'){
                   rawenbl();
                   memset(string,0,sizeof(string));
                   strcat(string, "RAW output activated");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   goto label1;
               }
              
               if(datat[0]=='I' && datat[1]=='R'){
                   rawdisbl();
                   memset(string,0,sizeof(string));
                   strcat(string, "RAW output inactivated");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   goto label1;
               }
              
                if(datat[0]=='G' && datat[1]=='A'){
                      __builtin_disable_interrupts();

                    LATBbits.LATB3=1;
                    delaymilli(1000);
                    Nop();
                    Nop();
                        __builtin_enable_interrupts();
                    GPSact=1;
                    activateTimer6();
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS output activated");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   goto label1;
               }
              
               if(datat[0]=='G' && datat[1]=='F'){
                    LATBbits.LATB3=1;
                    GpsFeed=1;
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS feedthrough activated");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   delaymilli(1000);
                   goto label1;
                  
               }
              
                   if(datat[0]=='G' && datat[1]=='X'){
                    LATBbits.LATB3=1;
                    GpsFeed=0;
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS feedthrough inactivated");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   goto label1;
               }
              
                   if(datat[0]=='G' && datat[1]=='T'){
                   if(GPSact==1){   
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS time / date: ");
                   sendUartCharArray(string);
                   memset(string,0,sizeof(string));
                   strcat(string, timestringSet);
                   sendUartCharArray(string);
                   memset(string,0,sizeof(string));
                   strcat(string, ":");
                   sendUartCharArray(string);  
                   memset(string,0,sizeof(string));
                   unsigned long nss=TMR6*10*timeMultiplicator;
                   sprintf (string, "%lu", nss);
                   sendUartCharArray(string);   
                  
                   memset(string,0,sizeof(string));
                   strcat(string, " / ");
                   sendUartCharArray(string);            
                   memset(string,0,sizeof(string));
                   strcat(string, datestringSet);
                   sendUartCharArray(string);  
                   }
                   else{
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS inactive");
                   sendUartCharArray(string);
                   }
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   goto label1;
               }
              
                   if(datat[0]=='G' && datat[1]=='I'){
                   GPSact=0;
                    inactivateTimer6();
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS output inactivated");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                    LATBbits.LATB3=0;

                   goto label1;
               }
              
                   if(datat[0]=='G' && datat[1]=='U'){
                   if(fix>1){   
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS fix: ");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = fix+0x30;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   }else{
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS NoFix");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;   
                   }
                   goto label1;
               }
              
                   if(datat[0]=='G' && datat[1]=='L'){
                   if(fix>1){   
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS location: ");
                   sendUartCharArray(string);
                   memset(string,0,sizeof(string));
                   strcat(string, locationstring);
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   }else{
                   memset(string,0,sizeof(string));
                   strcat(string, "GPS NoFix");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;   
                   }
                   goto label1;
               }
              
                   if(datat[0]=='G' && datat[1]=='C'){
                   if(GPSact==1 && fix>1){   
                   memset(string,0,sizeof(string));
                   strcat(string, "Timer calibration started, wait..");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   volatile int vall=PPStoCrystalCal();
                   if(vall<999999999 && vall > 149000000 && vall < 151000000){
                   memset(string,0,sizeof(string));
                   strcat(string, "Timer calibrated to ");
                   sendUartCharArray(string);
                   unsigned long nss=vall;
                   sprintf (string, "%lu", nss);
                   sendUartCharArray(string);
                   memset(string,0,sizeof(string));
                   strcat(string, "Hz");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;
                   }else{
                   memset(string,0,sizeof(string));
                   strcat(string, "Error: Calibration failed, signal dropped during calibration");
                   sendUartCharArray(string);
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0A;
                   while (U1STAbits.UTXBF);
                   U1TXREG = 0x0D;  
                   }
                   }else{
                       if (GPSact==0){
                            memset(string,0,sizeof(string));
                            strcat(string, "Error: GPS not activated");
                            sendUartCharArray(string);
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0A;
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0D;
                       }else{
                            memset(string,0,sizeof(string));
                            strcat(string, "Error: GPS no fix");
                            sendUartCharArray(string);
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0A;
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0D;                          
                       }
                   }
                   goto label1;
               }
              
              
               if(datat[0]=='P' && datat[1]=='T'){
                char substr1[strlen(datat)-2];
                strncpy(substr1, &datat[2],strlen(datat)-1);
                int pulses, frequency, voltage;
                sscanf(substr1, "%d:%d:%d", &pulses, &frequency, &voltage);
                Nop();
                Nop();
                send_pulsetrain(pulses,frequency,voltage);
                goto label1;
              } 

               if(datat[0]=='C' && datat[1]=='C'){
                            memset(string,0,sizeof(string));
                            cyclic=1;
                            strcat(string, "Cyclic output enabled");
                            sendUartCharArray(string);
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0A;
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0D;  
                goto label1;
              } 
              
                  if(datat[0]=='C' && datat[1]=='X'){
                            memset(string,0,sizeof(string));
                            cyclic=0;
                            strcat(string, "Cyclic output disabled");
                            sendUartCharArray(string);
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0A;
                            while (U1STAbits.UTXBF);
                            U1TXREG = 0x0D;  
                goto label1;
              } 
              
               if(datat[0]='C' && datat[1]=='T'){
                char substr1[strlen(datat)-2];
                strncpy(substr1, &datat[2],strlen(datat)-1);
                int pulses, fromfrequency, tofrequency, voltage;
                sscanf(substr1, "%d:%d:%d:%d", &pulses, &voltage, &fromfrequency, &tofrequency);
                send_chirp_pulsetrain(pulses,voltage,fromfrequency,tofrequency);
                goto label1;
              }  
              
               if(datat[0]='T' && datat[1]=='H'){
                char substr1[strlen(datat)-2];
                strncpy(substr1, &datat[2],strlen(datat)-1);
                int thressi=0;
                sscanf(substr1, "%d", &thressi);
                setthreshold(thressi);
               
                memset(string,0,sizeof(string));
                strcat(string, "Threshold set to ");
                char newstr[5];
                sprintf(newstr, "%d", thressi);
                strcat(string, newstr);
                               
                sendUartCharArray(string);
                while (U1STAbits.UTXBF);
                U1TXREG = 0x0A;
                while (U1STAbits.UTXBF);
                U1TXREG = 0x0D;
                goto label1;
               }
              
               if(datat[0]='O' && datat[1]=='I'){
                   setIOforTransmit();
                   tinkle();
                   setIOforSensing();
                goto label1;
               }
              
           }
          
          
          
        }   
           
      }
    }
    label1:
       if(U1STAbits.OERR==1) {
       U1STAbits.OERR=0;
   }
   if(U1STAbits.PERR==1) {
       U1STAbits.PERR=0;
   }
    Nop();
}

void rawenbl(void){
    rawenabled1=1;
    Nop();
}

void rawdisbl(void){
    rawenabled1=0;
}

//void printTemp (void)
//{
//volatile float tempvarr2=checktemp(2);
//volatile float tempvarr1=checktemp(1);
//memset(string,0,sizeof(string));
//strcat(string, "Temp1 = ");
//sendUartCharArray(string);
//
//sprintf (string, "%f", tempvarr1);
//sendUartCharArray(string);
//
//memset(string,0,sizeof(string));
//strcat(string, "degC - ");
//sendUartCharArray(string);
//
//memset(string,0,sizeof(string));
//strcat(string, "Temp2 = ");
//sendUartCharArray(string);
//
//sprintf (string, "%f", tempvarr2);
//sendUartCharArray(string);
//
//memset(string,0,sizeof(string));
//strcat(string, "degC");
//sendUartCharArray(string);
//
//while (U1STAbits.UTXBF);
//U1TXREG = 0x0A;
//while (U1STAbits.UTXBF);
//U1TXREG = 0x0D;
//}

/* MIPS32r2 byte-swap word */
#define _bswapw(x) __extension__({ \
 unsigned int __x = (x), __v; \
 __asm__ ("wsbh %0,%1;\n\t" \
 "rotr %0,16" /* template */ \
 : "=d" (__v) /* output */ \
 : "d" (__x)) /* input*/ ; \
 __v; \
})


int32_t main(void)
{
Nop();
Nop();
TRISB=0xFFFF;TRISC=0xFFFF;TRISD=0xFFFF;TRISE=0xFFFF;TRISF=0xFFFF;TRISG=0xFFFF;
Nop();
Nop();

 /*SCL_DIR = 0;
 SDA_PIN = 1;
 SDA_DIR = 0;
 SCL_PIN = 1;*/
delaymilli(1000);
setup_ports();
setup_pps();
init_uarts();
setIOforSensing();
//TRISEbits.TRISE5=0;
//LATEbits.LATE5=0;
LATBbits.LATB3=0;
delaymilli(100);
initInterrupts();
__builtin_enable_interrupts();
int is=0, ii=0, foundGPSstring = 0;
Nop();
Nop();

delaymilli(1000);
PR6=300000000;
//printString("Cadson sonar started."); printEL();
do{


//Nop();
//Nop();
//send_pulsetrain(20,46000,200);
//printTemp();

Nop();
Nop();
Nop();
Nop();
//volatile float tempvarr2=checktemp(2);   

//

//SYSKEY = 0x0; // Write invalid key to force lock
//SYSKEY = 0xAA996655; // Write Key1 to SYSKEY
//SYSKEY = 0x556699AA; // Write Key2 to SYSKEY
//PMD1=0x0000;PMD2=0x0000;PMD3=0x0000;PMD4=0x0000;PMD5=0x0000;PMD6=0x0000;PMD7=0x0000;

//OSCCONbits.SLPEN=1;
//SYSKEY = 0x0; // Write invalid key to force lock
//
//asm volatile( "wait" );



//delaymilli(10000);
//SPWMmodepulsetrain (47000,10);
Nop();
Nop();
//send_pulsetrain(20,47000,200);
//delaymilli(1000);
receivecommand();
    if (cyclic==0){
      //  send_pulsetrain(20,47000,250);
      // send_chirp_pulsetrain(800,30,45000,48000);
     //   setvoltage(350);
 //       SPWMmodepulsetrain(47000,20);
  //      SPWMmodechirp(47000,3);
//        delaymilli(10);
//Nop();
//    Nop();
    }
if(GPSact==1){

        if(U5STAbits.OERR==1){
            U5STAbits.OERR=0;
        }
        if(U5STAbits.URXDA!=0){
        char gpd=U5RXREG;
        if(GpsFeed==1){U1TXREG=gpd;}
        if(gpd== '$'){
            ii=0;
            if(gpsdata[1]=='G' && gpsdata[2]=='N' && gpsdata[3]=='R' && gpsdata[4]=='M' && gpsdata[5]=='C'){
                memset(timestring,0,sizeof(timestring));
                memset(datestring,0,sizeof(datestring));
                timestring[0]=gpsdata[7];timestring[1]=gpsdata[8];timestring[2]=gpsdata[9];
                timestring[3]=gpsdata[10];timestring[4]=gpsdata[11];timestring[5]=gpsdata[12];
                int ls=strlen(gpsdata) + 1;
                datestring[0]=gpsdata[ls-16];datestring[1]=gpsdata[ls-15];datestring[2]=gpsdata[ls-14];
                datestring[3]=gpsdata[ls-13];datestring[4]=gpsdata[ls-12];datestring[5]=gpsdata[ls-11];



            }
            if(gpsdata[1]=='G' && gpsdata[2]=='N' && gpsdata[3]=='G' && gpsdata[4]=='S' && gpsdata[5]=='A'){
                fix=0;
                fix=gpsdata[9]-0x30;
            }
            if(gpsdata[1]=='G' && gpsdata[2]=='N' && gpsdata[3]=='G' && gpsdata[4]=='G' && gpsdata[5]=='A'){
                    char *p;
                    char posi[20];
                    char ii=0;

                    p = strchr(gpsdata, ',');

                    while (p != NULL) {
                      posi[ii]=p - gpsdata + 1;
                      p = strchr(p + 1, ',');
                      ii++;
                    }
                      memcpy( locationstring, &gpsdata[posi[1]], posi[5]-posi[1]-1 );
                      locationstring[posi[5]-posi[1]-1] = '\0';
                      Nop();
            }           
           
           
            memset(gpsdata,0,sizeof(gpsdata));
       
        }
            gpsdata[ii]=gpd;
            ii++;
        }
}


//    U1TXREG='M';
//Nop();
//Nop();
//transmittimes();
//U1TXREG = 'M';
//delaymilli(5000);
 //   LATBINV=0b0010000000000000;
   
//   
//    LATDINV = 0b0000111000000001;
//    delaymillimicron(1);
}while(1);

return(1);
}



void delaymicro(int nn) //280 Nop() take about 149 clock cycles
{
	int mm = 0;
	do{
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
        Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		mm++;
	} while (mm<nn);
}

void delaymillimicron(int nn)
{
	int mm = 0;
	do{
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();
		mm++;
	} while (mm<nn);
}


void delaymilli(int nn)
{
	int kk = 0;
	do{
		delaymicro(1000);
		kk++;
	} while (kk<nn);
}

