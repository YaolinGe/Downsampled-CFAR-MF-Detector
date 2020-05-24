#include <ISBteensy.h>
#include <BaroSensor.h>
#include "SPI.h"
#include <Wire.h>
#include<MAX5419.h>
#include <Servo.h>
#include <SpiRam_Extended.h>
#include <Snooze.h>


//define stuff
#define RAM_SS_PIN 48

#define POWERADDR B1100111//Table 1 both LOW (7bit address)
#define NCP5623_ADDRESS 0x38

#define CHIP_SHUTDOWN             0b00000000
#define SET_MAX_LED_CURRENT       0b00100000
#define PWM_RED_LED               0b01000000
#define PWM_GREEN_LED             0b01100000
#define PWM_BLUE_LED              0b10000000
#define TARGET_UPWARD_IEND        0b10100000
#define TARGET_DOWNWARD_IEND      0b11000000
#define GRADUAL_DIMMING_STEP_TIME 0b11100000

//global stuff
SnoozeDigital digital;
SnoozeBlock config(digital);
SpiRAM SpiRam(SPI_CLOCK_DIV4, RAM_SS_PIN, CHIP_23LC1024);
MAX5419 D1(0x28);


//----------------------------------------------------------------
//----------------------------------------------------------------

// LED stuff
void NCP5623_Shutdown() {
  Wire.beginTransmission(NCP5623_ADDRESS);
  Wire.write(CHIP_SHUTDOWN);
  delayMicroseconds(1);
  Wire.endTransmission();
}

void NCP5623_Set_Led_Current(char value) {
  Wire.beginTransmission(NCP5623_ADDRESS);

  if ((value >= 0x00) && (value <= 0x1F)) {
    Wire.write(SET_MAX_LED_CURRENT | value);
  }
  delayMicroseconds(1);
  Wire.endTransmission();
}

void NCP5623_Set_Green_Led_Brightness(char value) {
  Wire.beginTransmission(NCP5623_ADDRESS);
  if ((value >= 0x00) && (value <= 0x1F)) {
    Wire.write(PWM_RED_LED | value);
  }
 delayMicroseconds(1);
  Wire.endTransmission();
}

void NCP5623_Set_Red_Led_Brightness(char value) {
  Wire.beginTransmission(NCP5623_ADDRESS);
  if ((value >= 0x00) && (value <= 0x1F)) {
    Wire.write(PWM_GREEN_LED | value);
  }
  delayMicroseconds(1);
  Wire.endTransmission();
}

void NCP5623_Set_Blue_Led_Brightness(char value) {
  Wire.beginTransmission(NCP5623_ADDRESS);
  if ((value >= 0x00) && (value <= 0x1F)) {
    Wire.write(PWM_BLUE_LED | value);
  }
  delayMicroseconds(1);
  Wire.endTransmission();
}

void NCP5623_Set_Gradual_Dimming_Upward(char value) {
  Wire.beginTransmission(NCP5623_ADDRESS);

  if ((value >= 0x00) && (value <= 0x1F)) {
    Wire.write(TARGET_UPWARD_IEND | value);
  }
  delayMicroseconds(1);
  Wire.endTransmission();
}

void NCP5623_Set_Gradual_Dimming_Downward(char value) {
  Wire.beginTransmission(NCP5623_ADDRESS);
  if ((value >= 0x00) && (value <= 0x1F)) {
    Wire.write(TARGET_DOWNWARD_IEND | value);
  }
  delayMicroseconds(1);
  Wire.endTransmission();
}

void NCP5623_Set_Gradual_Dimming_Step_Time(char value) {
  Wire.beginTransmission(NCP5623_ADDRESS);
  if ((value >= 0x00) && (value <= 0x1F)) {
    Wire.write(GRADUAL_DIMMING_STEP_TIME | value);
  }
  delayMicroseconds(1);
  Wire.endTransmission();
}
//----------------------------------------------------------------
//----------------------------------------------------------------



ISBteensy::ISBteensy(){ }  // Constructor
//----------------------------------------------------------------
void ISBteensy::setup(bool enablewatchDog){
  //set up I/O:s
  pinMode(BAR_PWRM_I2C_pwr, OUTPUT);
  pinMode(RS232_EN, OUTPUT);
  pinMode(POT_POWER, OUTPUT);
  pinMode(Vref_Enbl, OUTPUT);
  pinMode(VOL_OUT_enbl, OUTPUT);
  pinMode(POWER_GOOD, OUTPUT);
  pinMode(SLEWR_485 , INPUT);
  pinMode(RS485DE, OUTPUT);
  pinMode(RS232_485_SELECT, OUTPUT);
  pinMode(INVERTER_ENABLE, OUTPUT);
  pinMode(GREEN_LED_GND, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(POWER_M, OUTPUT);

  digitalWrite(BAR_PWRM_I2C_pwr, HIGH);
  digitalWrite(RS232_EN, HIGH);
  digitalWrite(POT_POWER, HIGH);
  digitalWrite(Vref_Enbl, LOW);
  digitalWrite(VOL_OUT_enbl, LOW);
  digitalWrite(POWER_GOOD, LOW);
  digitalWrite(RS485DE, HIGH);
  digitalWrite(RS232_485_SELECT, LOW);
  digitalWrite(INVERTER_ENABLE, HIGH);
  digitalWrite(GREEN_LED_GND, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(POWER_M, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);


  //Wire.begin(400000);
  //Wire.setClock(2000000);
  BaroSensor.begin();

  setVoltage(0.0);                           // Is this smart?
  temp         = getTemp();
  baro_hPa     = getBaro();
  energy       = 0.0;
  energy_L     = 0.0;
  loop_counter = 0;
  lastUpdate_energy   = micros(); // MilliSeconds
  lastUpdate_pressure = millis(); // MicroSeconds
  lastUpdate_temp     = millis(); // MilliSeconds

  init_LED();

  Wire.setClock(2000000); //as fast as possible

  if (enablewatchDog) {
    setupWatchDog();
  }
}
//----------------------------------------------------------------
void ISBteensy::update(){
  //update watchdog
  if(millis()- lastWatchDogUpdate > 100) {
    lastWatchDogUpdate = millis();
    noInterrupts();
    WDOG_REFRESH = 0xA602;
    WDOG_REFRESH = 0xB480;
    interrupts();
  }
  
  // Do not do this too often, it slows down everything since Captain needs to wait for reply...
	if (millis() - lastUpdate_temp > 5000) {  // Pretty seldom
    temp.value = getTemp();
    temp.ts = msyTime::getTimeStamp();
    lastUpdate_temp = millis();
    return; //just one update each loop
  }
	if (millis() - lastUpdate_pressure > 10000) { // Pretty seldom
    baro_hPa.value = getBaro();
    baro_hPa.ts = msyTime::getTimeStamp();
    lastUpdate_pressure = millis();
    return; //just one update each loop
  }
	if ((micros()-lastUpdate_energy)>500000){
	  voltage.value = getVoltage(); voltage.ts = msyTime::getTimeStamp();
	  current.value = getCurrent(); current.ts = msyTime::getTimeStamp();
	  integrateEnergy(voltage.value * current.value *(float)(micros()-lastUpdate_energy)/1000000);
	  lastUpdate_energy    = micros();
    return; //just one update each loop
	}
	if ((++loop_counter %100000)==0) {
     //set_LED_on_off(!LED_on_off);
     //if (LED_on_off==false){set_LED(0,1,0);}
     LED_on_off = !LED_on_off;
     set_LED_on_off(LED_on_off);
     digitalWrite(GREEN_LED,!LED_on_off);
     return; //just one update each loop
   }
}
//----------------------------------------------------------------
float ISBteensy::getVoltage(){
  byte ADCvinMSB, ADCvinLSB;
  unsigned int ADCvin;
  float inputVoltage;

  Wire.beginTransmission(POWERADDR);//first get Input Voltage - 80V max
  Wire.write(0x1E);
  Wire.endTransmission(false);
  Wire.requestFrom(POWERADDR, 2, true);
  //delay(1);
  ADCvinMSB = Wire.read();
  ADCvinLSB = Wire.read();
  ADCvin = ((unsigned int)(ADCvinMSB) << 4) + ((ADCvinLSB >> 4) & 0x0F);//formats into 12bit integer
  inputVoltage = ADCvin * 0.025; //25mV resolution

  return inputVoltage;
}
//------------------------------------------------------------------
void ISBteensy::setVoltage(float V){
  if(V<2.0){V=2.0;}
  V=V*1.010;
  int CalV=0;
  CalV = 752.6608*pow(V, -1.127);
  delay(10);
  Wire.begin();
  D1.write(CalV);
  delay(1);
  if(V==0) digitalWrite(VOL_OUT_enbl, LOW);          //Enable output voltage
  else digitalWrite(VOL_OUT_enbl, HIGH);             //Enable output voltage
}
//----------------------------------------------------------------
float ISBteensy::getTemp(){
  float temperature=0.0;
  //BaroSensor.begin();
  temperature = BaroSensor.getTemperature(TempUnit::CELSIUS, BaroOversampleLevel::OSR_256);
  //temperature = BaroSensor.getTemperature();
  return temperature;
}
//------------------------------------------------------------------
float ISBteensy::getBaro(){
  float bar=0.0;
  //BaroSensor.begin();
  //bar = BaroSensor.getPressure();
  bar = BaroSensor.getPressure(BaroOversampleLevel::OSR_256);
  return bar;
}
//------------------------------------------------------------------
float ISBteensy::getCurrent(){
  byte curSenseMSB, curSenseLSB;
  unsigned int ADCcur;
  float current =0.0;

  Wire.beginTransmission(POWERADDR);//get sense current
  Wire.write(0x14);
  Wire.endTransmission(false);
  Wire.requestFrom(POWERADDR, 2, true);
  //delay(1); //WHY?
  curSenseMSB = Wire.read();
  curSenseLSB = Wire.read();
  ADCcur = ((unsigned int)(curSenseMSB) << 4) + ((curSenseLSB >> 4) & 0x0F);//12 bit format
  //gets voltage across, 100uV resolution, then this converts to voltage for each sense resistor
  current = (ADCcur * 0.6)/1000.0;

  return current;
}
//----------------------------------------------------------------
void ISBteensy::init_LED(){
    NCP5623_Set_Led_Current(32);
    NCP5623_Set_Red_Led_Brightness(32);
    NCP5623_Set_Green_Led_Brightness(32);
    NCP5623_Set_Blue_Led_Brightness(32);
    NCP5623_Set_Gradual_Dimming_Upward(1);
    NCP5623_Set_Gradual_Dimming_Downward(1);
    NCP5623_Set_Gradual_Dimming_Step_Time(8);
}
//----------------------------------------------------------------
void ISBteensy::LED_shutdown() {
  //TODO LED
}
//----------------------------------------------------------------
void ISBteensy::set_LED_on_off(bool onoff) {
  //off -> RGB = 0
  //on R=R, B=B, G=G
  if(onoff) {
    NCP5623_Set_Red_Led_Brightness(RGB[0]);
    NCP5623_Set_Green_Led_Brightness(RGB[1]);
    NCP5623_Set_Blue_Led_Brightness(RGB[2]);

  }
  else {
    NCP5623_Set_Red_Led_Brightness(0);
    NCP5623_Set_Green_Led_Brightness(0);
    NCP5623_Set_Blue_Led_Brightness(0);
  }
}
//----------------------------------------------------------------
void ISBteensy::set_LED(uint8_t R, uint8_t G, uint8_t B) {
  RGB[0] = R;
  RGB[1] = G;
  RGB[2] = B;
}
//----------------------------------------------------------------
void ISBteensy::testRAM(void){
  SPI.begin();
  //SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  char data_to_chip[17] = "Testing 90123456";
  char data_from_chip[17] = "                ";
  int i = 0;

  // Write some data to RAM
  SpiRam.write_stream(SPI_CLOCK_DIV4, data_to_chip, 16);
  delay(100);

  // Read it back to a different buffer
  SpiRam.read_stream(SPI_CLOCK_DIV4, data_from_chip, 16);

  // Write it to the serial port
  for (i = 0; i < 16; i++) {
    Serial3.print(data_from_chip[i]);
  }
  Serial3.print("\n");
  delay(1000);                  // wait for a second
  SPI.end();
  pinMode(GREEN_LED, OUTPUT);
}
//----------------------------------------------------------------
void ISBteensy::integrateEnergy(float dE){
  energy_L =  energy_L + dE;
  if (energy_L>1.0){
	  energy.value   = energy.value + energy_L ; // [J] Fill the big bucket
    energy.ts = msyTime::getTimeStamp();
	  energy_L = 0.0;                // [J] Empty the small bucket
  }
}
//----------------------------------------------------------------
void ISBteensy::setupWatchDog(void) {
  // the following code should be placed at the end of setup() since the watchdog starts right after this
  WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
  WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
  delayMicroseconds(1); // Need to wait a bit..
  WDOG_STCTRLH = 0x0001;// Enable WDG
  WDOG_TOVALL = 0x0FFF; // 4096 ticks = 4s
  WDOG_TOVALH = 0;      // for longer times
  WDOG_PRESC = 0;       // This sets prescale clock so that the watchdog timer ticks at 1kHZ instead of the default 1kHZ/4 = 200 HZ
}
