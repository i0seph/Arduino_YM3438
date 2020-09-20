/*
 * this program is based https://github.com/MajicDesigns/MD_MIDIFile/tree/master/examples/MD_MIDIFile_Play
 * Thanks  MajicDesigns!
 * I changed MD_MIDIFile library for arduino progmem data.
 */
#include "ym3438.h"
#include "gmbank.h"
#include "overworld_4_.mid.h"
#include "MD_MIDIFile_progmem.h"

#define  DUMP_DATA         1

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


#define USE_MIDI  1   // set to 1 to enable MIDI output, otherwise debug output

#if USE_MIDI // set up for direct MIDI serial output

#define DEBUG(x)
#define DEBUGX(x)
#define DEBUGS(s)
#define SERIAL_RATE 31250

#else // don't use MIDI to allow printing debug statements

#define DEBUG(x)  Serial.print(x)
#define DEBUGX(x) Serial.print(x, HEX)
#define DEBUGS(s) Serial.print(F(s))
#define SERIAL_RATE 57600

#endif // USE_MIDI



MD_MIDIFile SMF;

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
#if USE_MIDI
/*
  if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
  {
    
    Serial.write(pev->data[0] | pev->channel);
    Serial.write(&pev->data[1], pev->size-1);
  }
  else
    Serial.write(pev->data, pev->size);
*/
  if(pev->data[0] == 0x90){
    Serial.print(pev->channel+1);
    Serial.print(",");
    Serial.print(pev->data[1]);
    Serial.print(",");
    Serial.println(pev->data[2]);
    if(pev->data[2] == 0) ym3438.noteOff(pev->channel+1);
    else ym3438.noteOn(pev->channel+1, (pev->data[1] / 12) - 1, pev->data[1] % 12, piano, pev->data[2]);
  }
#endif
  
  DEBUG("\n");
  DEBUG(millis());
  DEBUG("\tM T");
  DEBUG(pev->track);
  DEBUG(":  Ch ");
  DEBUG(pev->channel+1);
  DEBUG(" Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
    DEBUGX(pev->data[i]);
    DEBUG(' ');
  }
}


void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs 
// to be processed through the midi communications interface. Most sysex events cannot 
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{
  DEBUG("\nS T");
  DEBUG(pev->track);
  DEBUG(": Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
    DEBUGX(pev->data[i]);
    DEBUG(' ');
  }
}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  midi_event ev;

  // All sound off
  // When All Sound Off is received all oscillators will turn off, and their volume
  // envelopes are set to zero as soon as possible.
  ev.size = 0;
  ev.data[ev.size++] = 0xb0;
  ev.data[ev.size++] = 120;
  ev.data[ev.size++] = 0;

  for (ev.channel = 0; ev.channel < 16; ev.channel++)
    midiCallback(&ev);
}

void setup(void)
{
  Serial.begin(115200);
  SMF.begin(overworld_mid);
  SMF.setMidiHandler(midiCallback);
  SMF.setSysexHandler(sysexCallback);
   
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

  SMF.load(overworld_mid);
  //SMF.dump();

}

void loop(void)
{  
  SMF.getNextEvent();
}
