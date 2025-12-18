// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
/*
 * SimpleReceiverForHashCodes.cpp
 *
 * Demonstrates receiving hash codes of unknown protocols with IRremote
 *
 *  This file is part of Arduino-IRremote https://github.com/Arduino-IRremote/Arduino-IRremote.
 *
 ************************************************************************************
 * MIT License
 *
 * Copyright (c) 2024 Armin Joachimsmeyer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************************
 */

#include <Arduino.h>

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.

/*
 * Specify which protocol(s) should be used for decoding.
 * This must be done before the #include <IRremote.hpp>
 */
#define DECODE_HASH             // special decoder for all protocols
#if !defined(RAW_BUFFER_LENGTH)
#  if !((defined(RAMEND) && RAMEND <= 0x4FF) || (defined(RAMSIZE) && RAMSIZE < 0x4FF))
#define RAW_BUFFER_LENGTH  1000 // Especially useful for unknown and probably long protocols
#  endif
#endif
//#define DEBUG                 // Activate this for lots of lovely debug output from the decoders.

/*
 * This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
 */
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library



// Which pin on the Arduino is connecte      d 80koj9to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN_FL     4
#define LED_PIN_FR     5

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT  12

// NeoPixel brightness, 0 (min) to 255 (max)
//#define BRIGHTNESS 50 // Set BRIGHTNESS to about 1/5 (max = 255)
int BRIGHTNESS = 50; 

//#define COLORCOUNT 0  
int COLORCOUNT = 0; 
IRrecv irrecv(IR_RECEIVE_PIN);

decode_results results; 
unsigned long storedHexCode = 0; 

bool isOn = false; 

unsigned long lastTaskRunTime = 0;
const unsigned long taskInterval = 2000; // Run the task every 2 seconds


volatile bool naturalWhiteActive = false; 
volatile bool coolWhiteActive = false; 
volatile bool colorWipeActive = false; 
volatile bool theaterChaseActive = false; 

/*struct RGBWColor {
  uint8_t r; 
  uint8_t g; 
  uint8_t b; 
  uint8_t w; 
}
RGBColor currentColor; */

// Declare our NeoPixel strip object:
Adafruit_NeoPixel stripFrontLeft(LED_COUNT, LED_PIN_FL, NEO_GRBW + NEO_KHZ800);
Adafruit_NeoPixel stripFrontRight(LED_COUNT, LED_PIN_FR, NEO_GRBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

void setup() {
  Serial.begin(115200);

  // Just to know which program is running on my Arduino
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

  // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  //attachInterrupt(IR_RECEIVE_PIN, ISR, RISING);

  Serial.println(F("Ready to receive unknown IR signals at pin " STR(IR_RECEIVE_PIN) " and decode it with hash decoder."));
  stripFrontLeft.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  stripFrontLeft.show();            // Turn OFF all pixels ASAP
  stripFrontLeft.setBrightness(BRIGHTNESS);
  stripFrontRight.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  stripFrontRight.show();            // Turn OFF all pixels ASAP
  stripFrontRight.setBrightness(BRIGHTNESS);

  //fillOrange(); 
  irrecv.enableIRIn();
  fillNaturalWhite(); 
}

void loop() {

  /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded hash result is in IrReceiver.decodedIRData.decodedRawData
     */
    if (IrReceiver.available()) {
        IrReceiver.initDecodedIRData(); // is required, if we do not call decode();
        IrReceiver.decodeHash();
        IrReceiver.resume(); // Early enable receiving of the next IR frame
        /*
         * Print a summary and then timing of received data
         */
        //IrReceiver.printIRResultShort(&Serial);
        //IrReceiver.printIRResultRawFormatted(&Serial, true);

        Serial.println();

        /*
         * Finally, check the received data and perform actions according to the received command
         */
        auto tDecodedRawData = IrReceiver.decodedIRData.decodedRawData; // uint32_t on 8 and 16 bit CPUs and uint64_t on 32 and 64 bit CPUs
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            Serial.println(F("Repeat received. Here you can repeat the same action as before."));
            if(storedHexCode == 0xE5CFBD7F) 
            {
                // do something
                 Serial.println("The new brightness is: " + BRIGHTNESS); 
                 increaseBRIGHTNESS(); 
                 storedHexCode = 0; 
            }
            if(storedHexCode == 0xA3C8EDDB)
            {
               Serial.println("The new brightness is: " + BRIGHTNESS); 
                decreaseBRIGHTNESS(); 
                storedHexCode = 0; 
            }
        } else {
            //Serial.print(F("Raw data received are 0x"));
            //Serial.print("Hash code (Hex): ");
            //Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
            storedHexCode = IrReceiver.decodedIRData.decodedRawData; 
            Serial.print("The Stored Hash code is (Hex): ");
            Serial.println(storedHexCode, HEX);
            //Serial.println("The current hash is: " + IrReceiver.decodedIRData.command);
            //fillOrange();
            //cycleColors();
            //if (IrReceiver.decodedIRData.command == 0x3238126971) {

            //If you press the 0 button fill the LEDs with natural white
            if(storedHexCode == 0xC101E57B) {
                // do something
                fillNaturalWhite(); 
                storedHexCode = 0; 
            } else if (IrReceiver.decodedIRData.command == 0x11) {
                // do something else
            }
            //If you press the 1 button fill the LEDs with cool white
            if(storedHexCode == 0x9716BE3F) 
            {
                // do something
                //if(coolWhiteActive == true)
                //{
                  coolWhiteActive = !coolWhiteActive;
                //}
            }
            //If you press the 2 button fill LEDs with Red
            if(storedHexCode == 0x3D9AE3F7)
            {
                fillRed(); 
                storedHexCode = 0; 
            }
           //If you press the 3 button fill LEDs with Orange
            if(storedHexCode == 0x6182021B)
            {
                fillOrange(); 
                storedHexCode = 0; 
            }
            //If you press the 4 button fill LEDs with Yellow
            if(storedHexCode == 0x8C22657B)
            {
                fillYellow(); 
                storedHexCode = 0; 
            }
           //If you press the 5 button fill LEDs with Green
            if(storedHexCode == 0x488F3CBB)
            {
                fillGreen(); 
                storedHexCode = 0; 
            }
            //If you press the 6 button fill LEDs with Yellow
            if(storedHexCode == 0x449E79F)
            {
                fillBlue(); 
                storedHexCode = 0; 
            }
           //If you press the 7 button fill LEDs with Green
            if(storedHexCode == 0x32C6FDF7)
            {
                fillPink(); 
                storedHexCode = 0; 
            }
            //If you press the >>| button cycle through the rainbow colors from Red > Orange > ... >Blue > Pink
            if(storedHexCode == 0x20FE4DBB)
            {
                cycleColorsNext(); 
                storedHexCode = 0; 
            }
            //If you press the |<< button cycle throught the rainbow colors in reverse order Pink > Blue > ... > Orange > Red
            if(storedHexCode == 0xD7E84B1B)
            {
                cycleColorsPrevious(); 
                storedHexCode = 0; 
            }
            //If you press the power button turn off the LEDs without turning off the entire unity
            if(storedHexCode == 0xE318261B)
            {
                turnOff(); 
                storedHexCode = 0; 
            }
            //If you press the 8 button turn on a Black and White Marquee for 50 cycles
             //It has to finish its animation in order to move to a different thing. If you don't want to wait you have to turn it off with the switch
            if(storedHexCode == 0x1BC0157B)
            {
                theaterChase(stripFrontLeft.Color(0, 0, 0, 127), 500); // White, half brightness
                theaterChase(stripFrontRight.Color(0, 0, 0, 127), 500); // White, half brightness
                storedHexCode = 0; 
            }
            //If you press the 9 button turn on a Rainbow Marquee for 50 cycles
             //It has to finish its animation in order to move to a different thing. If you don't want to wait you have to turn it off with the switch
            if(storedHexCode == 0x3EC3FC1B)
            {
                theaterChaseRainbow(500); // White, half brightness
                theaterChaseRainbow(500); // White, half brightness
                storedHexCode = 0; 
            }

            //If you press the Loop button it turns on a rainbow cycle for all of the LEDs
            //It has to finish its animation in order to move to a different thing. If you don't want to wait you have to turn it off with the switch
            if(storedHexCode == 0x97483BFB)
            {
                rainbow(25); 
                storedHexCode = 0; 
            }

            /* This set of code was supposed to turn up and down the brightness, but I couldn't get it to work
            if(storedHexCode == 0xE5CFBD7F) 
            {
                // do something
                 Serial.println("The new brightness is: " + BRIGHTNESS); 
                 increaseBRIGHTNESS(); 
                 storedHexCode = 0; 
            }
            if(storedHexCode == 0xA3C8EDDB)
            {
               Serial.println("The new brightness is: " + BRIGHTNESS); 
                decreaseBRIGHTNESS(); 
                storedHexCode = 0; 
            } */ 
        }

        if(coolWhiteActive)
        {
          unsigned long currentTime = millis();
          if (currentTime - lastTaskRunTime >= taskInterval) 
          {
              lastTaskRunTime = currentTime;
              fillCoolWhite(); 
              storedHexCode = 0; 
              coolWhiteActive = false; 
          } 
        }
        
    }

  // Fill along the length of the strip in various colors...
  //colorWipe(stripFrontLeft.Color(255,   0,   0, 0)     , 50); // Red
  //colorWipe(stripFrontLeft.Color(  0, 255,   0, 0)     , 50); // Green
  //colorWipe(stripFrontLeft.Color(  0,   0, 255, 0)     , 50); // Blue
  //colorWipe(stripFrontLeft.Color(  0,   0,   0, 255), 50); // True white (not RGB white)

  //whiteOverRainbow(75, 5);

  //pulseWhite(5);

  //rainbowFade2White(3, 3, 1);

  //rainbow(20);
  //rainbowCycle(20);
  //theaterChase(stripFrontLeft.Color(127, 127, 127), 50); // White
  //theaterChase(stripFrontRight.Color(127, 127, 127), 50); // White
  //theaterChaseRainbow(50);
}
void IRAM_ATTR ISR() {
  // Code to handle the interrupt

}

void cycleColorsNext()
{ 
    switch (COLORCOUNT) {
      case 0: 
         fillRed(); 
         break; 
      case 1: 
         fillOrange(); 
         break; 
      case 2: 
         fillYellow(); 
         break; 
      case 3: 
         fillGreen(); 
         break; 
      case 4: 
         fillCyan(); 
         break; 
      case 5: 
         fillBlue(); 
         break; 
      case 6: 
         fillPurple(); 
         break; 
      case 7: 
         fillPink(); 
         break; 
    }
    COLORCOUNT += 1; 
    if(COLORCOUNT > 7)
    {
      COLORCOUNT = 0; 
    }
}
void cycleColorsPrevious()
{ 
    switch (COLORCOUNT) {
      case 0: 
         fillRed(); 
         break; 
      case 1: 
         fillOrange(); 
         break; 
      case 2: 
         fillYellow(); 
         break; 
      case 3: 
         fillGreen(); 
         break; 
      case 4: 
         fillCyan(); 
         break; 
      case 5: 
         fillBlue(); 
         break; 
      case 6: 
         fillPurple(); 
         break; 
      case 7: 
         fillPink(); 
         break; 
    }
    COLORCOUNT -= 1; 
    if(COLORCOUNT < 0)
    {
      COLORCOUNT = 7; 
    }
}

void fillRed()
{

  colorWipe(stripFrontLeft.Color(255,   0,   0, 0)     , BRIGHTNESS); // Red
  colorWipe(stripFrontRight.Color(255,   0,   0, 0)     , BRIGHTNESS); // Red
}

void fillOrange()
{
  colorWipe(stripFrontLeft.Color(255,   25,   0, 0)     , BRIGHTNESS); // Orange
  colorWipe(stripFrontRight.Color(255,   25,   0, 0)     , BRIGHTNESS); // Orange
}

void fillYellow()
{
  colorWipe(stripFrontLeft.Color(255,   255,   0, 0)     , BRIGHTNESS); // Yellow
  colorWipe(stripFrontRight.Color(255,   255,   0, 0)     , BRIGHTNESS); // Orange
}

void fillGreen()
{
  colorWipe(stripFrontLeft.Color(0,   255,   0, 0)     , BRIGHTNESS); // Green
  colorWipe(stripFrontRight.Color(0,   255,   0, 0)     , BRIGHTNESS); // Orange
}

void fillCyan()
{
  colorWipe(stripFrontLeft.Color(0,   255,   255, 0)     , BRIGHTNESS); // Blue
  colorWipe(stripFrontRight.Color(0,   255,   255, 0)     , BRIGHTNESS); // Orange
}

void fillBlue()
{
  colorWipe(stripFrontLeft.Color(0,   0,   255, 0)     , BRIGHTNESS); // Blue
  colorWipe(stripFrontRight.Color(0,   0,   255, 0)     , BRIGHTNESS); // Orange
}

void fillPurple()
{
  colorWipe(stripFrontLeft.Color(25,   0,   255, 0)     , BRIGHTNESS); // Purple
  colorWipe(stripFrontRight.Color(25,   0,   255, 0)     , BRIGHTNESS); // Orange
}

void fillPink()
{
  colorWipe(stripFrontLeft.Color(255,   0,   255, 0)     , BRIGHTNESS); // Pink
  colorWipe(stripFrontRight.Color(255,   0,   255, 0)     , BRIGHTNESS); // Orange
}

void fillCoolWhite()
{
  colorWipe(stripFrontLeft.Color(255, 255, 255, 0)    , BRIGHTNESS); // Cool Whtie
  colorWipe(stripFrontRight.Color(255,   255,   255, 0)     , BRIGHTNESS); // Orange
}

void fillNaturalWhite()
{
  colorWipe(stripFrontLeft.Color(0, 0, 0, 255)    , BRIGHTNESS); // Cool Whtie
  colorWipe(stripFrontRight.Color(0, 0, 0, 255)     , BRIGHTNESS); // Orange
}

void turnOff()
{
  colorWipe(stripFrontLeft.Color(0, 0, 0, 0)    , BRIGHTNESS); // Cool Whtie
  colorWipe(stripFrontRight.Color(0, 0, 0, 0)     , BRIGHTNESS); // Orange
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=stripFrontLeft.numPixels() - 1; i >= 0; i--) { // For each pixel in strip...
    stripFrontLeft.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    stripFrontRight.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    stripFrontLeft.show();                          //  Update strip to match
    stripFrontRight.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void whiteOverRainbow(int whiteSpeed, int whiteLength) {

  if(whiteLength >= stripFrontLeft.numPixels()) whiteLength = stripFrontLeft.numPixels() - 1;
  if(whiteLength >= stripFrontRight.numPixels()) whiteLength = stripFrontRight.numPixels() - 1;

  int      head          = whiteLength - 1;
  int      tail          = 0;
  int      loops         = 3;
  int      loopNum       = 0;
  uint32_t lastTime      = millis();
  uint32_t firstPixelHue = 0;

  for(;;) { // Repeat forever (or until a 'break' or 'return')
    for(int i=0; i<stripFrontLeft.numPixels(); i++) {  // For each pixel in strip...
      if(((i >= tail) && (i <= head)) ||      //  If between head & tail...
         ((tail > head) && ((i >= tail) || (i <= head)))) {
        stripFrontLeft.setPixelColor(i, stripFrontLeft.Color(0, 0, 0, 255)); // Set white
        stripFrontRight.setPixelColor(i, stripFrontRight.Color(0, 0, 0, 255)); // Set white
      } else {                                             // else set rainbow
        int pixelHue = firstPixelHue + (i * 65536L / stripFrontLeft.numPixels());
        stripFrontLeft.setPixelColor(i, stripFrontLeft.gamma32(stripFrontLeft.ColorHSV(pixelHue)));
        stripFrontRight.setPixelColor(i, stripFrontRight.gamma32(stripFrontRight.ColorHSV(pixelHue)));
      }
    }

    stripFrontLeft.show(); // Update strip with new contents
    stripFrontRight.show(); // Update strip with new contents
    // There's no delay here, it just runs full-tilt until the timer and
    // counter combination below runs out.

    firstPixelHue += 40; // Advance just a little along the color wheel

    if((millis() - lastTime) > whiteSpeed) { // Time to update head/tail?
      if(++head >= stripFrontLeft.numPixels()) {      // Advance head, wrap around
        head = 0;
        if(++loopNum >= loops) return;
      }
      if(++tail >= stripFrontLeft.numPixels()) {      // Advance tail, wrap around
        tail = 0;
      }
      lastTime = millis();                   // Save time of last movement
    }
  }
}
/*
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< stripFrontLeft.numPixels(); i++) {
      stripFrontLeft.setPixelColor(i, Wheel(((i * 256 / stripFrontLeft.numPixels()) + j) & 255));
      stripFrontRight.setPixelColor(i, Wheel(((i * 256 / stripFrontRight.numPixels()) + j) & 255));
    }
    stripFrontLeft.show();
    stripFrontRight.show();
    delay(wait);
  }
}*/

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) 
{
  for (int j=0; j < 50; j++) 
  {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) 
    {
      for (uint16_t i=0; i < stripFrontLeft.numPixels(); i=i+3) 
      {
        stripFrontLeft.setPixelColor(i+q, c);    //turn every third pixel on
        stripFrontRight.setPixelColor(i+q, c);    //turn every third pixel on
      }
      stripFrontLeft.show();
      stripFrontRight.show();

      delay(wait);

      for (uint16_t i=0; i < stripFrontLeft.numPixels(); i=i+3) 
      {
        stripFrontLeft.setPixelColor(i+q, 0);        //turn every third pixel off
        stripFrontRight.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) 
{
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a < 50; a++) 
  {  // Repeat 12 times...
    for(int b=0; b<3; b++) 
    { //  'b' counts from 0 to 2...
      stripFrontLeft.clear();         //   Set all pixels in RAM to 0 (off)
      stripFrontRight.clear();
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<stripFrontLeft.numPixels(); c += 3) 
      {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / stripFrontLeft.numPixels();
        uint32_t color = stripFrontLeft.gamma32(stripFrontLeft.ColorHSV(hue)); // hue -> RGB
        stripFrontLeft.setPixelColor(c, color); // Set pixel 'c' to value 'color'
        stripFrontRight.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      stripFrontLeft.show();                // Update strip with new contents
      stripFrontRight.show();
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}
void pulseWhite(uint8_t wait) 
{
  for(int j=0; j<256; j++) 
  { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected BRIGHTNESS level 'j':
    stripFrontLeft.fill(stripFrontLeft.Color(0, 0, 0, stripFrontLeft.gamma8(j)));
    stripFrontRight.fill(stripFrontRight.Color(0, 0, 0, stripFrontRight.gamma8(j)));
    stripFrontLeft.show();
    stripFrontRight.show();
    delay(wait);
  }

  for(int j=255; j>=0; j--) 
  { // Ramp down from 255 to 0
    stripFrontLeft.fill(stripFrontLeft.Color(0, 0, 0, stripFrontLeft.gamma8(j)));
    stripFrontRight.fill(stripFrontRight.Color(0, 0, 0, stripFrontRight.gamma8(j)));
    stripFrontLeft.show();
    stripFrontRight.show();
    delay(wait);
  }
}


void rainbowFade2White(int wait, int rainbowLoops, int whiteLoops) {
  int fadeVal=0, fadeMax=100;

  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {

    for(int i=0; i<stripFrontLeft.numPixels(); i++) { // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / stripFrontLeft.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (BRIGHTNESS) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      stripFrontLeft.setPixelColor(i, stripFrontLeft.gamma32(stripFrontLeft.ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
      stripFrontRight.setPixelColor(i, stripFrontRight.gamma32(stripFrontRight.ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
    }

    stripFrontLeft.show();
    stripFrontRight.show(); 
    delay(wait);

    if(firstPixelHue < 65536) {                              // First loop,
      if(fadeVal < fadeMax) fadeVal++;                       // fade in
    } else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) { // Last loop,
      if(fadeVal > 0) fadeVal--;                             // fade out
    } else {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }

  for(int k=0; k<whiteLoops; k++) {
    for(int j=0; j<256; j++) { // Ramp up 0 to 255
      // Fill entire strip with white at gamma-corrected BRIGHTNESS level 'j':
      stripFrontLeft.fill(stripFrontLeft.Color(0, 0, 0, stripFrontLeft.gamma8(j)));
      stripFrontRight.fill(stripFrontRight.Color(0, 0, 0, stripFrontRight.gamma8(j)));
      stripFrontLeft.show();
      stripFrontRight.show();
    }
    delay(1000); // Pause 1 second
    for(int j=255; j>=0; j--) { // Ramp down 255 to 0
      stripFrontLeft.fill(stripFrontLeft.Color(0, 0, 0, stripFrontLeft.gamma8(j)));
      stripFrontRight.fill(stripFrontRight.Color(0, 0, 0, stripFrontRight.gamma8(j)));
      stripFrontLeft.show();
      stripFrontRight.show();
    }
  }

  
  delay(500); // Pause 1/2 second
}

void increaseBRIGHTNESS() 
  {
    if(BRIGHTNESS < 255 )
    {
      BRIGHTNESS = BRIGHTNESS + 10; 
    }
    if(BRIGHTNESS > 255)
    {
       BRIGHTNESS = 255; 
    }
    Serial.println("The new brightness is: " + BRIGHTNESS); 
  }

  void decreaseBRIGHTNESS() 
  {
    if(BRIGHTNESS > 0 )
    {
      BRIGHTNESS = BRIGHTNESS - 10; 
    }
    if(BRIGHTNESS < 0)
    {
       BRIGHTNESS = 0; 
    }
     Serial.println("The new brightness is: " + BRIGHTNESS); 
  }

  // Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    // strip.rainbow() can take a single argument (first pixel hue) or
    // optionally a few extras: number of rainbow repetitions (default 1),
    // saturation and value (brightness) (both 0-255, similar to the
    // ColorHSV() function, default 255), and a true/false flag for whether
    // to apply gamma correction to provide 'truer' colors (default true).
    stripFrontLeft.rainbow(firstPixelHue);
    stripFrontRight.rainbow(firstPixelHue);
    // Above line is equivalent to:
    // strip.rainbow(firstPixelHue, 1, 255, 255, true);
    stripFrontLeft.show(); // Update strip with new contents
    stripFrontRight.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

