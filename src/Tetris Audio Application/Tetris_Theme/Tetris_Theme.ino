#include <pitches.h>

//Define the piezo pin
#define piezoPin 9

// Array for the notes
int melody[] = {
  NOTE_E3, NOTE_F3, NOTE_E3, NOTE_C3, NOTE_E3,  NOTE_C3, NOTE_G2,
  NOTE_A2, NOTE_A2, NOTE_C3, NOTE_E3, NOTE_D3, NOTE_C3,
  NOTE_G2, NOTE_G2, NOTE_C3, NOTE_D3, NOTE_E3,
  NOTE_C3, NOTE_A2, NOTE_A2,
  0, NOTE_D3, NOTE_G3, NOTE_A3, NOTE_G3, NOTE_G3,
  NOTE_E3, NOTE_E3, NOTE_C3, NOTE_E3, NOTE_D3, NOTE_C3,
  NOTE_G2, NOTE_F2, NOTE_C3, NOTE_D3, NOTE_E3,
  NOTE_C3, NOTE_A2, NOTE_A2, 0,
  NOTE_E3, NOTE_C3, NOTE_D3, NOTE_G2, NOTE_C3, NOTE_A2, NOTE_GS2, NOTE_G2,
  NOTE_E3, NOTE_C3, NOTE_D3, NOTE_G2, NOTE_C3, NOTE_E3, NOTE_A3, NOTE_GS3
};

// Array for the tempo
int tempo[] = {
  4,8,8,7,9,8,8,
  4,8,8,4,8,8,
  4,8,8,4,4,
  4,4,2,
  8,4,8,4,8,8,
  4,8,8,4,8,8,
  4,8,8,4,4,
  4,4,4,4,
  2,2,2,2,2,2,2,2,
  2,2,2,2,4,4,2,1
};

//********************************** Setup metheod **********************************
void setup(void)
{
   // Sets the piezo pin for output
   pinMode(9, OUTPUT);
}


//********************************** Loop method **********************************
void loop()
{
  // Plays the audio
  playMelody();
}


//********************************** Play melody method **********************************
// This method plays the Tetris melody
// It loops through the array setting each of the notes along with the tempo and note duration
// It then calls the buzz method to actualy generate the sound

void playMelody(){      

   int size = sizeof(melody) / sizeof(int);
   
   // This loops through the array of notes
   for (int currentNote = 0; currentNote < size; currentNote++) 
   {
       // Sets the note duration
       int noteDuration = 1000/tempo[currentNote];
       
       // Calls the buzz method, passing in the pin, the note and the note duration
       play(piezoPin, melody[currentNote],noteDuration);
       
       // Adds a delay
       delay(80);
       
       // Stops the tone playing
       play(piezoPin, 0,noteDuration); 
  }
}


//********************************** Play method **********************************
// This method is used to generate sound from a piezo element
// The method uses a loop to send high and low signals followed by a delay

void play(int pin, long frequency, long length) 
{
  // Sets the dealy value
  long delayValue = 1000000/frequency/2; 
  
  // Sets the number of cycles
  long numCycles = frequency * length/ 1000; 

  // This sends high and low signals to the piezo element in order to generate the sound
  for (long i=0; i < numCycles; i++)
  { 
    digitalWrite(pin,HIGH); 
    delayMicroseconds(delayValue); 
    digitalWrite(pin,LOW); 
    delayMicroseconds(delayValue);
  }
}
