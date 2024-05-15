#include <LiquidCrystal.h>

// Required libraries
#include <math.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Declare our NeoPixel strip object:
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
#define LED_PIN 9 
#define LED_COUNT 180
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const int buttonPin = 8;
int buttonState = 0;

unsigned long tick;
unsigned long previousTick = 0;
long interval = 500;
long speedChange = 500;

int speed;
int currentPos;
int lapCounter;

//Corners
int cornerPoints[3][2] = {
  {55, 60},
  {112, 130},
  {160, 163},
};


#define NOTE_D1  37
#define NOTE_C4 262
#define NOTE_D1 37
#define NOTE_B5  988

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// setup() function -- runs once at startup --------------------------------
void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  Serial.begin(9600);

  lcd.begin(16,2);
}

// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
  buttonState = digitalRead(buttonPin); //Gets the state of the button being pressed
  
  lcd.clear();
  lcd.print("LAP: ");
  lcd.print(lapCounter);

  //Updates the lcd to display the lap counter
  if(currentPos == 175 ? lapCounter = lapCounter + constrain(1, 1, 1) : false);

  //Prevents the car from moving if the car is at max speed on a corner
  if(!checkOnCorner()) {
    if (tick - previousTick >= interval) {
      previousTick = tick;
      currentPos = currentPos + 1;
      strip.clear();
        //Controls the car acceleration
        if (buttonState == HIGH) {
          if (interval > 100){ //The slot car will always move at least once every 100ms when the button is held
            interval = 100;
          }
        else {
          interval -= pow(interval, 0.65); //The formula for  calculating acceleration
        }
        //The max speed of the car
        if (interval < 7) { 
          interval = 7;
        }
        //Plays the max speed Tone
        if (interval < 8) {
          tone(7, NOTE_B5, 18);
        }
        //Plays the acceleration tone
        else {
          tone(7, NOTE_C4, 18);
        }
    } 
    //Controls the deceleration
    else {
      interval += 1; //Adds 1 to the interval when the button isn't held
      //Sets the slowest speed the slot car can go
      if (interval > 1000) {
        interval = 1000;
      }
      //Plays a tone when the button isnt being held
      tone(7, NOTE_D1, 18);
    }

    //Resets the LED when it reaches the end of the track
    if (currentPos > 175) {
      currentPos = 0;
    }
      }
  }
  else {
    for (int i = 0; i < 3; i++) { //Flashes 3 times
      tone(7, NOTE_D1, 18);
      //Sets the colors of the LEDS
      setCornerLEDS(cornerPoints[0][0], cornerPoints[0][1], false);
      strip.setPixelColor(currentPos, strip.Color(255, 0 ,255));
      strip.show();
      delay(250);

      //Clears the strip
      strip.clear();
      strip.show();
      delay(250);
    }
    interval = 100;
  }

  //Sets the lights for a corner
  //strip.setPixelColor(55, strip.Color(0, 128 ,0));
  //strip.setPixelColor(60, strip.Color(0, 128 ,0));

  setCornerLEDS(cornerPoints[0][0], cornerPoints[0][1], true);
  setCornerLEDS(112, 130, true);
  
  //Updates the LED car position
  strip.setPixelColor(currentPos, strip.Color(255, 0 ,255));

  strip.show();

  //Keeps track of the milliseconds passed
  tick = millis();
}

void setCornerLEDS(int start, int end, bool isGreen){
  for (int i = 0; i <= end - start; i++){
    if (isGreen){
      strip.setPixelColor(start + i, strip.Color(0, 128, 0));
    }
    else{
      strip.setPixelColor(start + i, strip.Color(255, 0, 0));
    }
  }
}

bool checkOnCorner() {
  for(int i = 0; i < 3; i++){
    if(currentPos >= cornerPoints[i][0] && currentPos <= cornerPoints[i][1] && interval <= 15)
    return true;
    else;
    return false;
  }
}