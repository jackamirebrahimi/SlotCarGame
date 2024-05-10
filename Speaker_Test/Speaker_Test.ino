#define NOTE_C3 131

tone(8, NOTE_C3, 4);

void setup() {
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(8, NOTE_C3, 4);

    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;

    delay(pauseBetweenNotes);
    noTone(8);
    }
}

void loop() {
  // put your main code here, to run repeatedly:

}
