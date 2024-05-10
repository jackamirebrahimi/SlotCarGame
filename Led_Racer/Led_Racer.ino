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
#define LED_PIN 6
#define LED_COUNT 120
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const int buttonPin = 2;
int buttonState = 0;

unsigned long tick;
unsigned long previousTick = 0;
long interval = 500;
long speedChange = 500;

int speed;
int currentPos;
int lapCounter;

#define NOTE_C4 262
#define NOTE_D1 37
#define NOTE_F7  2794

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
}


// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
  buttonState = digitalRead(buttonPin);
  
  if (tick - previousTick >= interval){
    previousTick = tick;
    currentPos = currentPos + 1;
    //Serial.println(previousTick);
    //Serial.println(pow(interval, 0.5));
    strip.clear();

    if (buttonState == HIGH) {

      if (interval > 100){
        interval = 100;
      }
      else{
        interval -= pow(interval, 0.65);
      }
      Serial.println(interval);
      if (interval < 5){
        interval = 5;
      }
      if (interval < 6){
        tone(8, NOTE_F7, 18);
      }
      else{
        tone(8, NOTE_C4, 18);
      }
    //Serial.println(currentPos);
  } else {
    interval += 1;
    if (interval > 1000){
      interval = 1000;
    }
    tone(8, NOTE_D1, 18);
  }
  }

  if (currentPos > 117) { //Resets the light when it reaches the end of the track
      currentPos = 0;
  }
  
  strip.setPixelColor(currentPos, strip.Color(255, 0 ,255));
  strip.show();
  //Show the LED
  if (interval < 1000){
  }

  tick = millis();
  
}

// One color being displayed at each led while erasing the other.
void oneColorAtOnce(uint32_t color, int wait) {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(50);
    strip.clear();
    wait = constrain(wait - 10, 5, 500);
  }
}