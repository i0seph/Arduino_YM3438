#include "ym3438.h"
#include "gmbank.h"


#define YM_IC (A0) // PC5 (= pin A5 for Arduino UNO)
#define YM_CS (A1) // PC4 (= pin A4 for Arduino UNO)
#define YM_WR (A2) // PC3 (= pin A3 for Arduino UNO)
#define YM_RD (A3) // PC2 (= pin A2 for Arduino UNO)
#define YM_A0 (A4) // PC1 (= pin A1 for Arduino UNO)
#define YM_A1 (A5) // PC0 (= pin A0 for Arduino UNO)

// Code -------------------------------
const int ledPin = 4;
const int freq = 8000000;
const int ledChannel = 0;
const int resolution = 3;

uint8_t cntrlpins[] = {YM_IC, YM_CS, YM_WR, YM_RD, YM_A0, YM_A1};
uint8_t datapins[] = {9,8,7,6,5,4,3,2}; 
Ym3438 ym3438(cntrlpins, datapins);

Instrument piano;
Instrument instrument_on_channel[6];
int current_seq = 0;

#if 0
void dump_instrument(Instrument inst){
  Serial.println("");
  Serial.println("-- Instrument Info --");
  Serial.print("feedback: ");
  Serial.println(piano.feedback); // 0..7, 0xb0
  Serial.print("algorithm: ");
  Serial.println(piano.algorithm); // 0..7, 0xb0
  Serial.print("LefOn: ");
  Serial.println(piano.enableLeft); // 0xb4
  Serial.print("RightOn: ");
  Serial.println(piano.enableRight); // 0xb4
  Serial.print("AM Rate: ");
  Serial.println(piano.amrate); // 0..3, 0xb4
  Serial.print("FM Rate: ");
  Serial.println(piano.fmrate); // 0..7, 0xb4
  Serial.println(" == Operators ==");
  Serial.println("      detune  multi  tlevel  keyscale  AMenable  attack  decay  susrate  suslevel  release  ssg");
  for(int i = 0; i < 4; i++){
    Serial.print("Op");
    Serial.print(i+1);
    Serial.print(":     ");
    Serial.print(piano.operators[i].detune); // 0..7, 0x30
    Serial.print(":     ");
    Serial.print(piano.operators[i].multi); // 0..15, 0x30
    Serial.print(":     ");
    Serial.print(piano.operators[i].tlevel); // 0..127, 0x40
    Serial.print(":     ");
    Serial.print(piano.operators[i].keyscale); // 0..3, 0x50
    Serial.print(":       ");
    Serial.print(piano.operators[i].enableam); // 0..31, 0x50
    Serial.print(":       ");
    Serial.print(piano.operators[i].attack); // 0x60
    Serial.print(":      ");
    Serial.print(piano.operators[i].decay); // 0.31, 0x60
    Serial.print(":      ");
    Serial.print(piano.operators[i].susrate); // 0.31, 0x70
    Serial.print(":      ");
    Serial.print(piano.operators[i].suslevel); // 0..15, 0x80
    Serial.print(":        ");
    Serial.print(piano.operators[i].release); // 0..15, 0x80
    Serial.print(":      ");
    Serial.print(piano.operators[i].ssg); // 0..7=disable, 8..15, 0x90  
    Serial.println("");  
  }  
}
#endif

void setup(void)
{
  Serial.begin(115200);
   
#if defined (ESP32)
  // 8mhz clock
  ledcSetup(ledChannel, freq, resolution);  
  ledcAttachPin(ledPin, ledChannel);  
  ledcWrite(ledChannel, 3);
#else
  // for 32Mhz
  pinMode(11, OUTPUT);
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  OCR2A = 1;
  TCCR2A |= (1 << WGM21);
  TCCR2A |= (1 << COM2A0);
  TCCR2B |= (1 << CS20);
  
  /*
  // for 16Mhz 
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  OCR2A = 0;
  TCCR2A |= (1 << WGM21);
  TCCR2A |= (1 << COM2A0);
  TCCR2B |= (1 << CS20);
  */
#endif

  uint8_t midipatchnum = 0;
  ym3438.reset();

  // all channel piano
  for(int i=0; i<6;i++) piano = ym3438.setInstrument(i+1, midipatchnum);

  //dump_instrument(piano);
}

void loop(void)
{  

  /*
    for(int k = 0; k < 128; k++){
      Serial.println(k);
      for(int chnum = 1; chnum < 7; chnum++){
        instrument_on_channel[chnum-1] = ym3438.setInstrument(chnum, k + (chnum - 1));
      }

      for(int j = 3; j<5; j++){  
        for(int i=0;i<12;i++) {
          if(i == 1 || i == 3 || i == 6 || i == 8 || i == 10) continue;
          for(int chnum = 1; chnum < 7; chnum++) ym3438.noteOn(chnum, j, i, instrument_on_channel[chnum-1], 100); // channel 1 Key on   
          delay(200);
          for(int chnum = 1; chnum < 7; chnum++) ym3438.noteOff(chnum); // channel 1 Key off
          delay(100); 
        }
      }
    }  
   */

/*
  for(int i = 1; i<5; i++) ym3438.setFMRate(i, piano, 0);
  for(int i = 1; i<5; i++) ym3438.setAlgorithm(i, piano, 7);
  // volume test
  //ym3438.noteOn(1, 4, 0);
 // ym3438.noteOn(2, 4, 4);
  ym3438.noteOn(3, 4, 7);
 // ym3438.noteOn(4, 5, 0);
  
  for(int i = 10; i < 128; i++){
    for(int j = 1; j < 5; j++){
      ym3438.setVelocity(j, piano, i);
    }
    delay(30);
  }
  for(int i = 126; i >= 10; i--){
    for(int j = 1; j < 5; j++){
      ym3438.setVelocity(j, piano, i);
    }
    delay(30);
  }
*/
  /*
  ym3438.noteOff(1);
  ym3438.noteOff(2);
  ym3438.noteOff(3);
  ym3438.noteOff(4);
  delay(100);
  */
  


  // velocity test
  int randNumber ;
  int notenum;
  int vol;
  bool isPlaying[7];

  for(int i=0; i<128; i++){
    //Serial.println(i);
    //piano = ym3438.setInstrument(1, i);

    /*
    ym3438.setLevel(1,1,0);
    ym3438.setLevel(1,2,127);
    ym3438.setLevel(1,3,0);
    ym3438.setLevel(1,4,0);
    */

    randNumber = random(31, 60);
    notenum = randNumber % 12;
    

    if((notenum == 1) || (notenum == 3) || (notenum == 5)  || (notenum == 6) || (notenum == 8) || (notenum == 10) || (notenum == 11)) continue;
    // 0, 2, 4, 7, 9
  
    
    piano = ym3438.setInstrument((i % 6) + 1, random(0, 100));
    ym3438.setLeft((i % 6) + 1, &piano, (bool)random(0,2));
    ym3438.setRight((i % 6) + 1, &piano, (bool)random(0,2));
    if(!piano.enableLeft && !piano.enableRight) {
      ym3438.setLeft((i % 6) + 1, &piano, 1);
      ym3438.setLeft((i % 6) + 1, &piano, 1);
    }
    
    if(isPlaying[(i % 6) + 1]) {
      ym3438.noteOff((i % 6) + 1);
      isPlaying[(i % 6) + 1] = true;
    }
    else{
      isPlaying[(i % 6) + 1] = false;
    }
    
    if((current_seq % 6) == 0) vol = 127;
    else if((current_seq % 6) == 3) vol = 110;
    else vol = random(60, 95);
    current_seq += 1;
    if(current_seq == 6) current_seq = 0;
    
    ym3438.noteOn((i % 6) + 1, (randNumber / 12) + 1, randNumber % 12, piano, vol);
    // Serial.print((i % 6));
    Serial.println(randNumber % 12);
    delay(220);
    //ym3438.noteOff((i % 6) + 1);
    //delay(10);
  }

}
