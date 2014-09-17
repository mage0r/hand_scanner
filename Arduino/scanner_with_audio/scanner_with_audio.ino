#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <CapacitiveSensor.h>

#include <Adafruit_NeoPixel.h>

#define TONE_PIN 6

#define LED_PIN 9

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, LED_PIN, NEO_GRB + NEO_KHZ800);


int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
// DIN = 11; CLK = 13
int numberOfHorizontalDisplays = 6;
int numberOfVerticalDisplays = 1;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);


// Cap sense variables.
int baseLine = 0 ; // set by the calibration method
int start = 0; // position in the text stream we're up to.
int capReadCounter = 0;
unsigned long capReadTime = 0;
unsigned long capUpdateTime = 0;
int randNumber;
unsigned long lastTextUpdate = 0;
int spacer = 1;
int width = 5 + spacer; // The font width is 5 pixels

// two colours we can set to run.
uint32_t color;
uint32_t color2;

CapacitiveSensor   cs_4_2 = CapacitiveSensor(A0,A1);        // 10M resistor between pins A0 & A1, pin A1 is sensor pin, add a wire and or foil if desired

/* editable settings */
unsigned long scanPeriod = 7000; // how long to hold the hand scanner.
unsigned long wait = 40; // In milliseconds - dictates the text scrolling speed

// Adding extra lines to this requires changes further down the line.
// Length for text strings can be changed no problem.
String messageArray[13] = { "Infection Sanctuary", "Place hand on scanner for bioscreening...",
                           "Scanning_/^\\_/^\\_/^\\_",
                           "No infection detected.","Proceed to processing.","Have a nice Day",
                           "Infection detected.","Medical teams notified.", "Proceed down the hall to DECON ROOM B.",
                           "Warning! Advanced Stage Infection detected.","Please Remain Calm.","Flame units enroute.",
                           "System Booting...Please Wait..................................."
                          };
                          


String credits[4] = { "Credits",
                      "John Spencer",
                      "Gavin Jackson",
                      "Gil Poznanski"
                    };


/* end editable settings */
                    
int systemStatus = 3; // 0 - waiting for input, 1 - scanning, 2 - infection.  3 - booting. 4 - credits
int messageStatus = 0; // matches the array positions above.

void setup() {
  
  Serial.begin(9600);
  
  pinMode(6, OUTPUT);
  beep(50);
  beep(50);
  beep(50);
  delay(1000);
  
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  CalibrateCap(200);
  
  randomSeed(analogRead(A0));
  
  //initialise our timers
  lastTextUpdate = millis();

  matrix.setIntensity(3); // Use a value between 0 and 15 for brightness

  // Adjust to your own needs
  matrix.setPosition(0, 0, 0); // The first display is at <0, 0>
  matrix.setPosition(1, 1, 0); // The second display is at <1, 0>
  matrix.setPosition(2, 2, 0); // The third display is at <2, 0>
  matrix.setPosition(3, 3, 0); // And the last display is at <3, 0>
  matrix.setPosition(4, 4, 0); // And the last display is at <3, 0>
  matrix.setPosition(5, 5, 0); // And the last display is at <3, 0>
  matrix.setPosition(6, 6, 0); // And the last display is at <3, 0>
  //  ...
  //  matrix.setRotation(0, 2);    // The first display is position upside down
  matrix.setRotation(0, 3);    // The same hold for the last display
  matrix.setRotation(1, 3);    // The same hold for the last display
  matrix.setRotation(2, 3);    // The same hold for the last display
  matrix.setRotation(3, 3);    // The same hold for the last display
  matrix.setRotation(4, 3);    // The same hold for the last display
  matrix.setRotation(5, 3);    // The same hold for the last display
  matrix.setRotation(6, 3);    // The same hold for the last display

  strip.begin();
  strip.show();

 
}

void loop() {
  
  long total1 =  cs_4_2.capacitiveSensor(30);
  
  // if the touch sensor is active.
  // get an out of range error if we don't wait a bit before starting.
  if (total1 > baseLine && millis() > scanPeriod) {
    Serial.println("Hand detected");
    Serial.println(systemStatus);
    if(systemStatus == 0) { // system status is 0, displaying base message.
      capReadTime = millis(); // start the detection timer.
      capUpdateTime = millis();
      systemStatus = 1;
      messageStatus = 2;
      start = 0; // reset the text stream
      strip.setPixelColor(capReadCounter, strip.Color(0, 0, 255));
      capReadCounter++;
    } else if (systemStatus == 1) {
      if(capUpdateTime < (millis() - (scanPeriod/16))) { //add another counter.
        capUpdateTime = millis();
        strip.setPixelColor(capReadCounter, strip.Color(0, 0, 255));
        capReadCounter++;
      }
      
      if(capReadTime < (millis() - scanPeriod)) { // touch sensor has been active long enough
        //capReadTime = millis(); // this
        systemStatus = 2; // run infection subroutine
        
        
        randNumber = random(0,4);
        start=0; // reset the text start.
        
        // set up the infection level.
        if(randNumber == 1) { // minor infection.
          Serial.println("minor infection");
          messageStatus = 6; // first minor infection.
          beep(50);
          color = strip.Color(255,0,0);
          color2 = strip.Color(255,0,0);
          
        } else if (randNumber == 2) { //major infection
          Serial.println("major infection");
          messageStatus = 9; // first minor infection.
                  beep(100);              
                  beep(200);
                  beep(100);
                  beep(200);
                  beep(100);
                  beep(200);
                  beep(100);              
                  beep(200);
                  beep(100);
                  beep(200);
                  beep(100);
                  beep(200);
                  beep(100);
                  beep(200);
                  beep(100);
                  beep(200);
                  beep(100);
                  
          color = strip.Color(255,0,0);
          color2 = strip.Color(50,0,0);
          
        } else { // 3 or 4 are clean.
          Serial.println("no infection");
          messageStatus = 3; // first minor infection.
               beep(900);
               beep(900);              
                  beep(900);
                  delay(60);
                  beep(100);
                  beep(100);
                  beep(100);
                  delay(60);
                  beep(900);
                  beep(900);
                  beep(900);
                  delay(60);
          color = strip.Color(0,255,0);
          color2 = strip.Color(0,255,0);
        }
      }
    }
    
    if(systemStatus != 4 && millis() > 20000 && capReadTime < (millis() - 20000)) { // still holding!  Credits!
      systemStatus = 4;
      messageStatus = 0;
      start = 0;
      matrix.write(); // Send bitmap to display
      color = strip.Color(0,255,0);
      color2 = strip.Color(0,0,255);
    }
      
    // if it's another of the options, do nothing
  } else if(capReadCounter && capUpdateTime < (millis() - (scanPeriod/16))) { // we've let go of the sensor!
    // only really need to act on this if we've already in the touch status.
    // capReadCounter is how many leds on the neopixel.
    capUpdateTime = millis();
    capReadCounter--;
    strip.setPixelColor(capReadCounter+1, strip.Color(0, 0, 0));
    strip.setPixelColor(capReadCounter, strip.Color(0, 0, 0));
    
    
    if(systemStatus == 1) {
      systemStatus = 0; // reset the system status.
      start = 0;
      messageStatus = 0;
    }
  }

  // display the selected text.
  // this is where our advance timer goes.
  if (lastTextUpdate < (millis() - wait)) {
    lastTextUpdate = millis(); // update the last time we did a text update
    
    // which text are we displaying?
    if(systemStatus == 3) { // This is our boot sequence.
      scrollText(messageArray[12], start); // start the text scrolling
      if(start >= width * messageArray[messageStatus].length() + matrix.width() - 1 - spacer)
        start = 0;
      else
        start++;
      
      if(millis() > scanPeriod)
      {
        systemStatus = 0;
        start = 0;
      }
        
    } else if(systemStatus == 0) { // initial text
    
      scrollText(messageArray[messageStatus], start); // start the text scrolling
      if(start >= width * messageArray[messageStatus].length() + matrix.width() - 1 - spacer)
      {
        // this is a little bit hacky.  only two options for the first set
        if(messageStatus >= 1)
          messageStatus = 0;
        else
        
          messageStatus = 1;
          
        start = 0;
      }
      else
        start++;
        
    } else if (systemStatus == 1) { // scanning
      scrollText(messageArray[messageStatus], start); // start the text scrolling.  It's always 2 for this option
      if(start >= width * messageArray[messageStatus].length() + matrix.width() - 1 - spacer)
        start = 0;
      else
        start++;
    } else if (systemStatus == 2) { //response.
    
      running_dots(); // rotate our array.
    
      capReadCounter = 0;
      scrollText(messageArray[messageStatus], start); // start the text scrolling
      if(start >= width * messageArray[messageStatus].length() + matrix.width() - 1 - spacer)
      {
        
          if(messageStatus == 8 || messageStatus == 11 || messageStatus == 5) // we've finished this sequence!  Reset
          {
            messageStatus = 0;
            systemStatus = 0;
            
            //turn off our neopixels.
            for(int i = 15;i>=0;i--)
              strip.setPixelColor(i, strip.Color(0, 0, 0));
          }
          else
            messageStatus++;
           
        
        start = 0; // reset string position.
      }
      else
        start++;
    } else if (systemStatus == 4){ // roll credits!
      running_dots(); // rotate our array.
      
        capReadCounter = 0;
        
        scrollText(credits[messageStatus], start); // start the text scrolling.  It's always 2 for this option
        if(start >= width * credits[messageStatus].length() + matrix.width() - 1 - spacer)
        {
          
            if(messageStatus == 3) // we've finished this sequence!  Reset
            {
              messageStatus = 0;
              systemStatus = 0;
              
              //turn off our neopixels.
              for(int i = 15;i>=0;i--)
                strip.setPixelColor(i, strip.Color(0, 0, 0));
            }
            else
              messageStatus++;
             
          
          start = 0; // reset string position.
        }
        else
          start++;
    }
  }

  strip.show(); // update the contents of the neopixel array.

}

void scrollText(String tape, int pos) {
  //for ( int i = 0 ; i < width * tape.length() + matrix.width() - 1 - spacer; i++ ) {

    matrix.fillScreen(LOW); // clears the screen.

    int letter = pos / width;
    int x = (matrix.width() - 1) - pos % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically

    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < tape.length() ) {
        matrix.drawChar(x, y, tape[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display

  //}

}

void running_dots() {
  // is the led on or off
  uint16_t start = 0;
  uint16_t finish = 15;
  
  unsigned int toggle = 1;
  if (strip.getPixelColor(start) == strip.getPixelColor(start+1))
    toggle = 0;
  else if ( strip.getPixelColor(start+1) == strip.getPixelColor(start+2))
    toggle = 2;
  
    
    for(uint16_t i=start; i<=finish; i++) {
      if (toggle == 0 )
          strip.setPixelColor(i, color);
      else
          strip.setPixelColor(i, color2);
          
      toggle++;
      if ( toggle > 2)
        toggle = 0;
      
    }
}


void CalibrateCap(int offset)
{
  int sample = 0 ;
  
  // take 30 samples and return the highest value for the pin
     for( int j = 0 ; j < 10 ; j++ )
     {
       sample = cs_4_2.capacitiveSensor(30);
       if( sample > baseLine)
       {
         baseLine = sample ;
       }
     }
     
  baseLine = baseLine + offset ;
} // end of calibrate

void beep(unsigned char delayms){
  analogWrite(6, 200);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(6, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}  

