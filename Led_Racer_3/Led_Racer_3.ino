// Required libraries
#include <math.h>
#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>
#include <Servo.h>
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

Servo flagServo;

const int buttonPin = 8;
int buttonState = 0;

unsigned long tick;
unsigned long previousTick = 0;
long interval = 500;
long speedChange = 500;

int speed;
int currentPos;

int currentTime;
int minuteTime;
int secondTime;

int lapCounter= 1;

String lapText = "LAP: ";
String timerText = "TIME: ";
bool minutePassed = false;
bool raceFinished = false;

//Corners
int cornerPoints[5][2] = {
  {27, 34},
  {53, 60},
  {92, 99},
  {138, 145},
  {152, 158}
};
int arrayLength = 5;

//Music
#define NOTE_D1  37
#define NOTE_C4 262
#define NOTE_D1 37
#define NOTE_B5  988

int introSong[] = {
  55, 55, 44, 330, 262, 330, 392, 262, 262, 262, 294
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  8, 8, 4, 6, 6, 6, 2, 6, 6, 6, 1
};

int startSong[] = {
  294, 294, 294, 262
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int startDurations[] = {
  2, 2, 2, 2
};

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

  flagServo.attach(10);
  flagServo.write(0);

  introSequence();
}

// loop() function -- runs repeatedly as long as board is on ---------------
void loop() {
  buttonState = digitalRead(buttonPin); //Gets the state of the button being pressed

  //Prevents the car from moving if the car is at max speed on a corner
  if(!checkOnCorner()) {
    carMovementManager();
  }
  else {
    for (int i = 0; i < 3; i++) { //Flashes 3 times
      carPenalty();
    }
    interval = 100;
  }

  lightAndTimeManager();

  currentTime = (tick / 1000) - 8;
  
  if(currentTime % 60 == 0 && !minutePassed && currentTime > 59) {
    minuteTime = minuteTime + 1;
    minutePassed = true;
  }
  if(currentTime % 61 == 0) {
    minutePassed = false;
  }

  secondTime = (currentTime % 60);

  lcd.print(lapText + lapCounter + " / 7");
  lcd.setCursor(0, 1);

  if(secondTime < 10 ? lcd.print(timerText + minuteTime + ":" + "0" + secondTime) : lcd.print(timerText + minuteTime + ":" + secondTime));

  lcd.setCursor(0, 0);

  if(lapCounter == 8 && !raceFinished) {
    raceFinished = true;
    scoreGame();
  }
}

void setCornerLEDS(int start, int end, bool isGreen) {
  for (int i = 0; i <= end - start; i++){
    if (isGreen){
      strip.setPixelColor(start + i, strip.Color(255, 95, 0));
    }
    else{
      strip.setPixelColor(start + i, strip.Color(255, 0, 0));
    }
  }
}

bool checkOnCorner() {
  for(int i = 0; i < arrayLength; i++){
    if(currentPos >= cornerPoints[i][0] && currentPos <= cornerPoints[i][1] && interval <= 12){
      return true;
    }
  }
  return false;
}

void carMovementManager() {
  if (tick - previousTick >= interval) {
      previousTick = tick;
      currentPos = currentPos + 1;
      strip.clear();

      //Updates the lcd to display the lap counter
      if(currentPos == 175 ? lapCounter = lapCounter + constrain(1, 1, 1) : false);

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

void carPenalty() {
  tone(7, NOTE_D1, 18);
      //Sets the colors of the LEDS
      for(int i = 0; i < arrayLength; i++){
        setCornerLEDS(cornerPoints[i][0], cornerPoints[i][1], false);
      }
      strip.show();
      delay(250);

      //Clears the strip
      strip.clear();
      strip.show();
      delay(250);
}

void lightAndTimeManager() {
  //Sets the lights for a corner
  for(int i = 0; i < arrayLength; i++){
    setCornerLEDS(cornerPoints[i][0], cornerPoints[i][1], true);
  }

  strip.setPixelColor(175, strip.Color(255, 255 ,255));
  strip.setPixelColor(0, strip.Color(255, 255 ,255));

  //Updates the LED car position
  strip.setPixelColor(currentPos, strip.Color(255, 0 ,255));

  strip.show();

  //Keeps track of the milliseconds passed
  tick = millis();
}

void introSequence() {
  lcd.print("SLOT LED RACING");
  for (int thisNote = 0; thisNote < 11; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];

    tone(7, introSong[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);
    noTone(8);
  }
  lcd.clear();

  delay(1250);
  for (int thisNote = 0; thisNote < 11; thisNote++) {
    int noteDuration = 1000 / startDurations[thisNote];

    tone(7, introSong[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);
    noTone(8);
  }

  flagServo.attach(10);
  flagServo.write(90);
  delay(250);
  flagServo.detach();
}

int scoreGame() {
  lcd.clear();
  lcd.print("RACE OVER!");
  lcd.setCursor(0, 1);
  if(secondTime < 10 ? lcd.print(timerText + minuteTime + ":" + "0" + secondTime) : lcd.print(timerText + minuteTime + ":" + secondTime));

  flagServo.attach(10);
  for(int i = 0; i < 6; i++) {
    flagServo.write(90);
    delay(250);
    flagServo.write(0);
    delay(250);
  }
  flagServo.detach();

  delay(7000);
  asm volatile("jmp 0");
}