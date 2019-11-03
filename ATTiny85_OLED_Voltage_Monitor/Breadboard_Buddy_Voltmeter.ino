
// ATTiny85 OLED 128x64 Text Voltmeter Display
// Using the following libraries:
//
// https://github.com/kirknorthrop/SSD1306_minimal
// https://github.com/adafruit/TinyWireM
//
// Install ATTiny board manager https://github.com/damellis/attiny
// Set board type to ATTiny25/45/85 in Arduino IDE
// Set processor to ATTiny85, Clock internal 8 MHz.
// Bootloader was burned from the IDE using Arduino (UNO) as ISP and ATTiny85 on a breadboard,
// then this sketch was uploaded also using Arduino as ISP.
// ATTiny85 fuses were manually set to LFuse E1, HFuse 5D, EFuse FE using a high voltage fuse programmer
// (confirm what this does other than making the reset pin into a normal GPIO so it can be an ADC input)
//
// Compile results in Arduino IDE 1.8.10
// Sketch uses 5064 bytes (61%) of program storage space. Maximum is 8192 bytes.
// Global variables use 78 bytes (15%) of dynamic memory, leaving 434 bytes for local variables. Maximum is 512 bytes.
//
// Gadget Reboot
//

#include "SSD1306_minimal.h"

// Scaling multipliers to convert the ADC numerical reading of 0-1023 into the actual
// voltage being measured from the power supplies, accounting for the voltage dividers at the ADC inputs.
// eg.  To measure 0-30v on a 0-5v ADC input, a voltage divider with 56K to VIN and 10K to GND will divide
//      the input voltage by 6.6.
//      For a 10 bit ADC with 0-5V input range, each analog reading interval represents 4.88mV.  (5V / 1024 = 4.88mV)
//      To scale the analog reading back to the pre-voltage divider voltage, multiply the analog reading by
//      (4.88mV * 6.6)
//      So if the analog reading is 256, the power supply rail voltage is (256 * 4.88mV * 6.6) = 8.25V

#define scale_30 0.03223  // (5/1024) * 6.6 = 0.03223 for voltage divider 56K and 10K, Vin = 0-30V
#define scale_15 0.01465  // (5/1024) * 3   = 0.01465 for voltage divider 20K and 10K, Vin = 0-15V

#define VSW_Pin  0         // VSW  voltage rail is monitored on analog 0
#define VCC_Pin  2         // VCC  voltage rail is monitored on analog 2
#define VADJ_Pin 3         // VADJ voltage rail is monitored on analog 3

SSD1306_Mini oled;         // oled object

char strValue[5];          // for converting float to string for text display

void setup() {
  pinMode(1, INPUT);       // sw bodge: set PB1 as input due to PCB v1.0 layout wiring botch on SCL net.
  TinyWireM.begin();       // init I2C library
  oled.init(0x3c);         // init OLED display
  oled.clear();
  oled.cursorTo(0, 0);
  oled.printString( "Voltage Rails");
  oled.cursorTo(0, 18);
  oled.printString( "VSW: ");
  oled.cursorTo(0, 36);
  oled.printString( "VADJ: ");
  oled.cursorTo(0, 54);
  oled.printString( "VCC: ");

}


void loop() {

  // read VSW voltage rail
  oled.cursorTo(36, 18);
  printAnalog(VSW_Pin, scale_30);
  oled.printString(" V     ");

  // read VADJ voltage rail
  oled.cursorTo(36, 36);
  printAnalog(VADJ_Pin, scale_30);
  oled.printString(" V     ");

  // read VCC voltage rail
  oled.cursorTo(36, 54);
  printAnalog(VCC_Pin, scale_15);
  oled.printString(" V     ");

  // pause between ADC readings so small changes don't flicker too fast
  // possible future improvement - instead of wasting time delaying,
  // take multiple readings and average them for less fluctuation and data flicker
  delay(300);  

}

// read the requested ADC input and calculate the voltage with 2 decimals
// print as text on OLED display
void printAnalog(byte i, float scale) {
  dtostrf((analogRead(i) * scale), 4, 2, strValue);
  oled.printString(strValue);
}
