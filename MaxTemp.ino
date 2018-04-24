#include "PlayingWithFusion_MAX31856.h"
#include "PlayingWithFusion_MAX31856_STRUCT.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

uint8_t TC0_CS  = 3;
uint8_t TC1_CS  =  10;

double tmp;
#define TFT_DC 9
#define TFT_CS 10

unsigned long time;
bool instant =  true;
bool last = false;
int current_time = 0;
int temp_latest ;

int temp_initial = 0;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

PWF_MAX31856  thermocouple0(TC0_CS);

void setup()
{
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  delay(1000);                            // give chip a chance to stabilize
  Serial.begin(115200);                   // set baudrate of serial port
  // setup for the the SPI library:
  SPI.begin();                            // begin SPI
  SPI.setClockDivider(SPI_CLOCK_DIV16);   // SPI speed to SPI_CLOCK_DIV16 (1MHz)
  SPI.setDataMode(SPI_MODE3);             // MAX31856 is a MODE3 device
  
  // call config command... options can be seen in the PlayingWithFusion_MAX31856.h file
  thermocouple0.MAX31856_config(K_TYPE, CUTOFF_60HZ, AVG_SEL_4SAMP, CMODE_AUTO);

  



}

void loop()
{
  time = millis();
  static struct var_max31856 TC_CH0;
  
  delay(100);
  struct var_max31856 *tc_ptr;
  
  // Read CH 0
  tc_ptr = &TC_CH0;                             // set pointer
  thermocouple0.MAX31856_update(tc_ptr);        // Update MAX31856 channel 0
  
  
  // ##### Print information to serial port ####
  
  // Thermocouple channel 0
  Serial.print("Thermocouple 0: ");            // Print TC0 header
  if(TC_CH0.status)
  {
    // lots of faults possible at once, technically... handle all 8 of them
    // Faults detected can be masked, please refer to library file to enable faults you want represented
    Serial.println("fault(s) detected");
    Serial.print("Fault List: ");
    if(0x01 & TC_CH0.status){Serial.print("OPEN  ");}
    if(0x02 & TC_CH0.status){Serial.print("Overvolt/Undervolt  ");}
    if(0x04 & TC_CH0.status){Serial.print("TC Low  ");}
    if(0x08 & TC_CH0.status){Serial.print("TC High  ");}
    if(0x40 & TC_CH0.status){Serial.print("TC Range  ");}
    Serial.println(" ");
  }
  else  // no fault, print temperature data
  {
    // MAX31856 External (thermocouple) Temp
    tmp = (double)TC_CH0.lin_tc_temp * 0.0078125;           // convert fixed pt # to double
    Serial.print("TC Temp = ");                   // print TC temp heading
    Serial.println(tmp);
  }

  if((time-current_time)>200 && (instant==true))
  {
    if((tmp - temp_initial)>150)
    {
      temp_latest = tmp;
      instant =  false;
      current_time = time;
    }
    else
    {
      current_time = time;
    }
  }

  if((instant==false)&&((time-current_time)>1000))
  {
    if((tmp-temp_latest)<5)
    {
      last = true; 
    }
    else
    {
      current_time =time;
      temp_latest=tmp;
    }
  }

  
  if(last==true)
  {
   
    tft.begin(); 
    testText();   
  }
  
}


unsigned long testText() 
{

  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setRotation(3);
  tft.setCursor(5, 75);
  tft.setTextSize(12);
  int tmp_1 = tmp;
  tft.println(tmp_1);


}

