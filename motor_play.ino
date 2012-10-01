/**
* Copyright (C) 2012 Interactive Lab
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
* and associated documentation files (the "Software"), to deal in the Software without restriction, 
* including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
* subject to the following conditions:
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
* INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
* Note frequencies and formulas from http://www.phy.mtu.edu/~suits/notefreqs.html
**/

#define MOTORS 4
#define NOTE_LENGTH 100000 // in microseconds
#define NOTES_NUM 28
#define A 1.059463094359 // 2^(1/12)

int delays[NOTES_NUM];

// b  c  c# d  d# e  f  f# g  g# a  a# b  c  c# d  d# e  f  f# g  g# a  a# b  c  c#
//
// |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  
// v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  v  
//
// a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {
  
                                                                               //v
char music[] =  "000000000000000noooo000qrrrr000noo0qrr0wvv0orr0vuuuuuuuuutromooo000000000000000noooo000qrrrr000noo0qrr0wvv0rvv0{zzzzzzzzzzzzzz00";
char music2[] = "cc00cc00cc00cc00cc00cc00ff00ff00cc00cc00jj00ff00ii00ii00gg00gg00ee00ee00cc00aa00cc00cc00ff00ff00cc00ff00jj00jj00nn00nn00nn00nn00";
char music3[] = "000000000000000bcccc000effff000bcc0eff0kjj0cff0jiiiiiiiiihfcaccc000000000000000bcccc000effff000bcc0eff0kjj0fjj0onnnnnnnnnnnnnn00";
char music4[] = "000000000000000bcccc000effff000bcc0eff0kjj0cff0jiiiiiiiiihfcaccc000000000000000bcccc000effff000bcc0eff0kjj0fjj0onnnnnnnnnnnnnn00";
int currentNote = 0;
int musicLength;

void setup() {
  delays[0] = 0;
  float freq = 493.88; // starting from B4
  for (int i = 1; i < NOTES_NUM; i++) {
    freq = freq * A;
    delays[i] = 1e6 / (freq * 2);
  }
  
  for (int i = 0; i < MOTORS; i++) {
    pinMode(i*2 + 2, OUTPUT);
    pinMode(i*2 + 3, OUTPUT);
  }
  
  musicLength = sizeof(music) - 1; // 0x0
}

void loop() {
  if (currentNote >= musicLength) currentNote = 0;
  
  int notes[MOTORS];
  notes[0] = music[currentNote] - 96; // charCode('a') == 97
  notes[1] = music2[currentNote] - 96;
  notes[2] = music3[currentNote] - 96;
  notes[3] = music4[currentNote] - 96;
  playNote(notes);
  currentNote++;
}

void playNote(int values[]) {
  int noteDelays[MOTORS];
  int noteDelaysLeft[MOTORS];
  boolean enabled[MOTORS];
  
  for (int i = 0; i < MOTORS; i++) {
    if (values[i] < 0) values[i] = 0;
    noteDelays[i] = delays[values[i]];
    noteDelaysLeft[i] = noteDelays[i];
    enabled[i] = false;
  }
  
  long time = NOTE_LENGTH;
  unsigned long lastTime = micros();
  unsigned long deltaTime;
  while (time > 0) {
    deltaTime = micros() - lastTime;
    lastTime = micros();
    
    for (int i = 0; i < MOTORS; i++) {
      if (noteDelays[i] < 100) continue; // skip delays which break the stepper
      
      long leftToWait = noteDelaysLeft[i] - deltaTime;
      if (leftToWait <= 0) {
        noteDelaysLeft[i] = noteDelays[i];
        if (enabled[i]) {
          enabled[i] = false;
          digitalWrite(i*2 + 3, LOW);
        } else {
          if (time > noteDelays[i]) {
            enabled[i] = true;
            digitalWrite(i*2 + 3, HIGH);
          }
        }
      } else {
        noteDelaysLeft[i] = leftToWait;
      }
    }
    time -= deltaTime;
  }
  
  for (int i = 0; i < MOTORS; i++) {
    if (enabled[i] == true) {
      digitalWrite(i*2 + 3, LOW);
    }
  }
}
