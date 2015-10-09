/*******************************************
 Melody

This example shows how to make a short melody
with Sparki's buzzer.
********************************************/
#include <Sparki.h> // include the sparki library
#include "pitches.h" // include a list of pitches

// notes in the melody:
int melody[] = { NOTE_CS4, NOTE_F4, NOTE_DS4, NOTE_FS3, 0, NOTE_G4, NOTE_AS4, NOTE_A4,NOTE_D4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = { 2, 2, 2, 2, 2, 2, 2, 2, 2 };

void setup() {
  // play each note in the arrays
  
}

void loop() {
  // no need to repeat the melody.
  
  int code = 0;
  code = sparki.readIR();
  
  if(code == 70){
    for (int thisNote = 0; thisNote < 9; thisNote++) {
  
      // calculate the note duration as 1 second divided by note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000/noteDurations[thisNote];
      sparki.beep(melody[thisNote],noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      sparki.noBeep();
    }
  }
  delay(500);
}
