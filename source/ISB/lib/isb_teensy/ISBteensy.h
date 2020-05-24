#ifndef ISB_TEENSY_H
#define ISB_TEENSY_H

/*------------------------------------------------------------------------------------
  Intelligent Sensor Bridge, ISB
  V2.0
------------------------------------------------------------------------------------*/
#include "Arduino.h"
#include <msyTime.h>

#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

//----------------------------------------------------------------
class ISBteensy{

public:
  ISBteensy();
  void  setup(bool enablewatchDog=true);           // Initializes the pins etc
  void  update();                                  // Do some energy integration etc (every loop)
  void  setVoltage(float V);                       // Set the sensor-voltage
  void  set_LED(uint8_t R, uint8_t G, uint8_t B);  // Sets the LED to the RGB value
  void  testRAM(void)	;                            //
  float getEnergy();

  //pins for ISB thing
  int  POWER_GOOD               = 00;    //NOT USED YET!
  int  BAR_PWRM_I2C_pwr         = 42;    //POWER TO DEVICES ON BOARD
  int  RS232_EN                 = 44;
  int  POT_POWER                = 41;    //POWER TO VOLTAGE REGULATING POT
  int  RS232_485_SELECT         = 50;    //LOW FOR 232 AND HIGH FOR 485 OUT
  int  SLEWR_485                = 53;    //FOR SLEEP TO AVOID LISTENING TO 2Mbit
  int  RS485DE                  = 51;    //DRIVER ENABLE
  int  Vref_Enbl                = 46;    //ADC PRECISION VREF
  int  VOL_OUT_enbl             = 47;    //OUTPUT VOLTAGE ENABLE
  int  INVERTER_ENABLE          = 40;    //INVERTER POWER for RS-485
  int  GREEN_LED                = 13;    //General purpose LED
  int  GREEN_LED_GND            = 57;    //General purpose LED
  int  POWER_M                  = 43;    //POWER MEASURE ENBL.


  // Some variables for ISB-states
  uint8_t RGB[3] = {0,1,0};                         // default LED colors
  TimestapmedValue<float>  voltage  = 0.0;          // System voltage fed to ISB
  TimestapmedValue<float>  current  = 0.0;          // Current fed to ISB [Amp]
  TimestapmedValue<float>  energy   = 0.0;          // [J] Large Energy consumption bucket
  float energy_L = 0.0;                             // [J] Small Energy consumption bucket
  TimestapmedValue<float>  temp     = 0.0;          // ISB temp measured in barometer [°C]
  TimestapmedValue<float>  baro_hPa = 0.0;          // Barometric pressure [hPA]
  unsigned long lastUpdate_energy;                  //
  unsigned long lastUpdate_pressure;                //
  unsigned long lastUpdate_temp;                    //
  unsigned long loop_counter;
  float         voltage_setpoint=0.0;
  bool          LED_on_off=true;

  const  uint8_t red   = 0b00000001;
  const  uint8_t green = 0b00000010;
  const  uint8_t blue  = 0b00000100;

private:
  // These are private since they are updated in the update function
  float getVoltage();               // Reads the supplied voltage
  float getTemp();                  // Deg C
  float getBaro();                  // HPa
  float getCurrent();               // Ampere
  void  LED_shutdown();
  void  init_LED();
  void  set_LED_on_off(bool onoff); //Toggle LED
  void  integrateEnergy(float dE);
  void  setupWatchDog();            //Setup the watchdog

  uint32_t lastWatchDogUpdate = 0;


};
//----------------------------------------------------------------
#endif
