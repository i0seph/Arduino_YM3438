/* 
 * ym3438 library for arduino 
 * base https://github.com/theacodes/genesynth/blob/master/firmware/genesynth/ym2612.cpp (thanks theacodes)
 */

#include "ym3438.h"
#include "gmbank.h"

#define DEBUG 0
#if DEBUG
#include "arduino.h"
#endif

#define USE_DIRECTGPIO 1

Ym3438::Ym3438(uint8_t cntrlpins[], uint8_t dpins[])
{
  for(int i = 0; i < 6; i++) pinMode(cntrlpins[i], OUTPUT);
  pinIC = cntrlpins[0];
  pinCS = cntrlpins[1];
  pinWR = cntrlpins[2];
  pinRD = cntrlpins[3];
  pinA0 = cntrlpins[4];
  pinA1 = cntrlpins[5];
  for(int i=0; i<8;i++) {
    pinMode(dpins[i], OUTPUT);
    datapins[i] = dpins[i];
  }
}

void Ym3438::reset()
{
#if USE_DIRECTGPIO 
  // yma1 a5, yma0 a4, a3 rd, a2 wr, a1 cs, a0 ic
  PORTC |= 0b1111;
  PORTC &= 0b11001111;
  delayMicroseconds(20);
  PORTC &= 0b11111110;
  delayMicroseconds(20);
  PORTC |= 0b1;  
#else
  digitalWrite(pinA0, LOW);
  digitalWrite(pinA1, LOW);
  digitalWrite(pinCS, HIGH);
  digitalWrite(pinWR, HIGH);
  digitalWrite(pinRD, HIGH);
  
  digitalWrite(pinIC, HIGH);
  delayMicroseconds(5);
  digitalWrite(pinIC, LOW);
  delayMicroseconds(5);
  digitalWrite(pinIC, HIGH);
#endif
}

void Ym3438::setreg(uint8_t reg, uint8_t data1, bool enableA1)
{
#if DEBUG
  Serial.print("Register: ");
  Serial.print(reg, HEX);
  Serial.print(", Data: ");
  Serial.print(data1, HEX);
  Serial.print(", A1: ");
  Serial.println(enableA1);
#endif

#if USE_DIRECTGPIO
  /*
    B (digital pin 8 to 13) 8-9
    C (analog input pins)
    D (digital pins 0 to 7) 2-7  
   */
  if(enableA1) PORTC |= 0b100000;
  else PORTC &= 0b11011111;

  PORTC &= 0b11101101;

  // 2 -> 7, 3 -> 6, 4 -> 5, 5 -> 4, 6 -> 3, 7-> 2
  PORTD = (PORTD & 0b11) | ((reg&4) << 5) | ((reg&8) << 3) | ((reg&16) << 1) | ((reg&32)>> 1) | ((reg&64) >> 3) | ((reg&128) >> 5);
  PORTB = (PORTB & 0b11111100) | ((reg&1) << 1) | ((reg&2) >> 1) ; // 0 -> 9, 1 -> 8
  delayMicroseconds(5);
  
   PORTC &= 0b11111011;
   delayMicroseconds(5);
   PORTC |= 0b100;
   delayMicroseconds(5);
 
   PORTC |= 0b10010;
   PORTC &= 0b11111101;
  
  // PORTA = data;
  PORTD = (PORTD & 0b11) | ((data1&4) << 5) | ((data1&8) << 3) | ((data1&16) << 1) | ((data1&32)>> 1) | ((data1&64) >> 3) | ((data1&128) >> 5);
  PORTB = (PORTB & 0b11111100) | ((data1&1) << 1) | ((data1&2) >> 1) ;
  delayMicroseconds(5);
  
   PORTC &= 0b11111011;
   delayMicroseconds(5);
   PORTC |= 0b100;
   delayMicroseconds(5);

   PORTC |= 0b10; 
   
#else
  digitalWrite( pinA1 , enableA1 ? HIGH : LOW );
  
  digitalWrite( pinA0 , LOW ); 
  digitalWrite( pinCS , LOW );
  
  //PORTA = reg;
  digitalWrite(datapins[0], (reg&1)?HIGH:LOW);
  digitalWrite(datapins[1], (reg&2)?HIGH:LOW);
  digitalWrite(datapins[2], (reg&4)?HIGH:LOW);
  digitalWrite(datapins[3], (reg&8)?HIGH:LOW);
  digitalWrite(datapins[4], (reg&16)?HIGH:LOW);
  digitalWrite(datapins[5], (reg&32)?HIGH:LOW);
  digitalWrite(datapins[6], (reg&64)?HIGH:LOW);
  digitalWrite(datapins[7], (reg&128)?HIGH:LOW);
  
  delayMicroseconds(1);
  
  digitalWrite( pinWR , LOW );
  delayMicroseconds(5);
  digitalWrite( pinWR , HIGH );
  delayMicroseconds(5);

  
  digitalWrite( pinCS , HIGH );
  
  
  digitalWrite( pinA0 , HIGH ); 
  digitalWrite( pinCS , LOW );
  
  // PORTA = data;
  digitalWrite(datapins[0], (data1&1)?HIGH:LOW);
  digitalWrite(datapins[1], (data1&2)?HIGH:LOW);
  digitalWrite(datapins[2], (data1&4)?HIGH:LOW);
  digitalWrite(datapins[3], (data1&8)?HIGH:LOW);
  digitalWrite(datapins[4], (data1&16)?HIGH:LOW);
  digitalWrite(datapins[5], (data1&32)?HIGH:LOW);
  digitalWrite(datapins[6], (data1&64)?HIGH:LOW);
  digitalWrite(datapins[7], (data1&128)?HIGH:LOW);
  delayMicroseconds(1);
  
  digitalWrite( pinWR , LOW );
  delayMicroseconds(5);
  digitalWrite( pinWR , HIGH );
  delayMicroseconds(5);
  
  digitalWrite( pinCS , HIGH );
#endif
}

void Ym3438::getreg(uint8_t reg, uint8_t *data1, bool enableA1) {
   *data1 = 100;
  digitalWrite( pinA1 , enableA1 ? HIGH : LOW );
  
  digitalWrite( pinA0 , LOW ); 
  digitalWrite( pinCS , LOW );
  
  //PORTA = reg;
  digitalWrite(datapins[0], (reg&1)?HIGH:LOW);
  digitalWrite(datapins[1], (reg&2)?HIGH:LOW);
  digitalWrite(datapins[2], (reg&4)?HIGH:LOW);
  digitalWrite(datapins[3], (reg&8)?HIGH:LOW);
  digitalWrite(datapins[4], (reg&16)?HIGH:LOW);
  digitalWrite(datapins[5], (reg&32)?HIGH:LOW);
  digitalWrite(datapins[6], (reg&64)?HIGH:LOW);
  digitalWrite(datapins[7], (reg&128)?HIGH:LOW);
  
  delayMicroseconds(1);
  
  digitalWrite( pinRD , LOW );
  delayMicroseconds(5);
  digitalWrite( pinWR , HIGH );
  delayMicroseconds(5);

  
  
  digitalWrite( pinCS , HIGH );
  
  
  digitalWrite( pinA0 , HIGH ); 
  digitalWrite( pinCS , LOW );

   //                 765432                 98
   //                 234567                 01
  // *data1 = (PORTD & 0b11111100) | (PORTB & 0b11);
  *data1 = ((PORTB & 0b10) >> 1) | ((PORTB & 1) << 1) | ((PORTD & 0x80) >> 5) | ((PORTD & 0x40) >> 3) | ((PORTD & 0x20) >> 1) | ((PORTD & 0x10) << 1) |  ((PORTD & 0x8) << 3) | ((PORTD & 0x4) << 5);
  digitalWrite( pinRD , HIGH );
  digitalWrite( pinCS , HIGH );
}

void Ym3438::noteOff(uint8_t channel){  
  setreg(0x28, ((channel < 4) ? channel - 1 : channel), false);
}

void Ym3438::noteOn(uint8_t channel, uint8_t octave, uint8_t keynum ){
  setreg(0xA4 + ((channel - 1) % 3), octave << 3 | (fnum[keynum] / 0xff), (channel > 3) ? true : false); 
  setreg(0xA0 + ((channel - 1) % 3), fnum[keynum] & 0xff, (channel > 3) ? true : false);
  
  setreg(0x28, (15 << 4) | ((channel < 4) ? channel - 1 : channel), false);
}

void Ym3438::noteOn(uint8_t channel, uint8_t octave, uint8_t keynum, Instrument inst, uint8_t velocity ){
  setVelocity(channel, inst, velocity);
  noteOn(channel, octave, keynum);
}

void Ym3438::setLFO(bool isset, uint8_t hznum){
  setreg(0x22, isset << 3 | (hznum & 7 ), false);
}

void Ym3438::setLevel(uint8_t channel, uint8_t opnum, uint8_t vol){
  // ym3438.setreg(0x40, 0x23);
  
    setreg(0x40 + ((channel - 1)%3) + oparr[inter_opnum[opnum]], 127 - vol, (channel > 3) ? true : false );
}

void Ym3438::setVelocity(uint8_t channel, Instrument inst, uint8_t vol) {
  // vol = 0 (quietest) .. 127 (loudest) 
  // quiest is all op 127
  // loudest is all op default tlevel
  
  int optlevel;
 //float volpercent = (float)vol * ((float)100 / (float)127);
 float volpercent = pgm_read_float(&velocity_percent[vol]);

#if DEBUG
  Serial.println(volpercent);
#endif
  int newlevel;
  for(int i = 1; i < 5; i++){
    optlevel = 127 - inst.operators[i-1].tlevel;
    newlevel = round((float)optlevel * (volpercent * 0.01));
    setLevel(channel, i, newlevel);
  }
}

void Ym3438::setFeedback(uint8_t channel, Instrument *inst, uint8_t value) { 
  inst->feedback = value & 7;
  setreg(0xB0 + ((channel - 1) % 3), (inst->feedback << 3) | inst->algorithm, (channel > 3) ? true : false);
}

void Ym3438::setAlgorithm(uint8_t channel, Instrument *inst, uint8_t value) { 
  inst->algorithm = value & 7;
  setreg(0xB0 + ((channel - 1) % 3), (inst->feedback << 3) | inst->algorithm, (channel > 3) ? true : false);
}

void Ym3438::setLeft(uint8_t channel, Instrument *inst, bool value) {
  inst->enableLeft = value;
  setreg(0xB4 + ((channel - 1) % 3), (inst->enableLeft << 7) | (inst->enableRight << 6) | (inst->amrate << 4) | (inst->fmrate), (channel > 3) ? true : false); // AM/FM
}

void Ym3438::setRight(uint8_t channel, Instrument *inst, bool value) {
  inst->enableRight = value;
  setreg(0xB4 + ((channel - 1) % 3), (inst->enableLeft << 7) | (inst->enableRight << 6) | (inst->amrate << 4) | (inst->fmrate), (channel > 3) ? true : false); // AM/FM
}

void Ym3438::setAMRate(uint8_t channel, Instrument *inst, uint8_t value) {
  inst->amrate = value & 3;
  setreg(0xB4 + ((channel - 1) % 3), (inst->enableLeft << 7) | (inst->enableRight << 6) | (inst->amrate << 4) | (inst->fmrate), (channel > 3) ? true : false); // AM/FM
}

void Ym3438::setFMRate(uint8_t channel, Instrument *inst, uint8_t value) {
  inst->fmrate = value & 7;
  setreg(0xB4 + ((channel - 1) % 3), (inst->enableLeft << 7) | (inst->enableRight << 6) | (inst->amrate << 4) | (inst->fmrate), (channel > 3) ? true : false); // AM/FM
}

// set operator
void Ym3438::setDetune(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].detune = value & 7;
  setreg(0x30 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].detune << 4) | (inst->operators[opnum-1].multi), (channel > 3) ? true : false);
}

void Ym3438::setMulti(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){  
  inst->operators[opnum-1].multi = value & 15;
  setreg(0x30 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].detune << 4) | (inst->operators[opnum-1].multi), (channel > 3) ? true : false);
}

void Ym3438::setKeyscale(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].keyscale = value & 3;
  setreg(0x50 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].keyscale << 7) | (inst->operators[opnum-1].attack), (channel > 3) ? true : false);
}

void Ym3438::setAttack(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].keyscale = value & 3;
  setreg(0x50 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].keyscale << 7) | (inst->operators[opnum-1].attack), (channel > 3) ? true : false);
}

void Ym3438::setAm(uint8_t channel, uint8_t opnum, Instrument *inst, bool value){
  inst->operators[opnum-1].enableam = value;
  setreg(0x60 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].enableam << 8) | (inst->operators[opnum-1].decay), (channel > 3) ? true : false);
}

void Ym3438::setDecay(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].decay = value & 31;
  setreg(0x60 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].enableam << 8) | (inst->operators[opnum-1].decay), (channel > 3) ? true : false);
}

void Ym3438::setSusrate(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].susrate = value & 31;
  setreg(0x70 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].susrate), (channel > 3) ? true : false);
}

void Ym3438::setSuslevel(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].suslevel = value & 15;
  setreg(0x80 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].suslevel << 4) | (inst->operators[opnum-1].release), (channel > 3) ? true : false);
}

void Ym3438::setRelease(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].release = value & 15;
  setreg(0x80 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].suslevel << 4) | (inst->operators[opnum-1].release), (channel > 3) ? true : false);
}

void Ym3438::setSsg(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value){
  inst->operators[opnum-1].ssg = value & 15;
  setreg(0x90 + oparr[inter_opnum[opnum]] + ((channel - 1)%3), (inst->operators[opnum-1].susrate), (channel > 3) ? true : false);
}

Instrument Ym3438::setInstrument(uint8_t channel, uint8_t instnum){
  Instrument tempinst;
  int j = 2;
  uint8_t opreg[7] = {0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90};
  int tempbyte;
  int lopnum;

  
  // setreg(0x22, 9, false); // LFO
  setLFO(true, 1); // enable lfo, 5.56hz

  tempbyte = pgm_read_byte_near(gmbank_hex[instnum]);
  setreg(0xB0 + ((channel - 1) % 3), tempbyte , (channel > 3) ? true : false); // Feedback/algorithm
  tempinst.feedback = (tempbyte & 0b111000) >> 3;
  tempinst.algorithm = (tempbyte & 0b111);

  tempbyte = pgm_read_byte_near(gmbank_hex[instnum] + 1);
  setreg(0xB4 + ((channel - 1) % 3), 0xc0 | tempbyte, (channel > 3) ? true : false); // AM/FM
  tempinst.enableLeft = true;
  tempinst.enableRight = true;
  tempinst.amrate = (tempbyte & 0b110000) >> 4;
  tempinst.fmrate = (tempbyte & 0b111);
  
  for(int k = 0; k < 4; k++){
    for(int i = 0; i < 7 ; i++){
      tempbyte = pgm_read_byte_near(gmbank_hex[instnum] + j);
      
      setreg(opreg[i] + oparr[k] + ((channel - 1)%3), tempbyte, (channel > 3) ? true : false);

      if(k==1) lopnum = 2;
      else if(k==2) lopnum = 1;
      else lopnum = k;
      
      switch(i){
        case 0:
        tempinst.operators[lopnum].detune = (tempbyte & 0b1110000) >> 4;
        tempinst.operators[lopnum].multi = (tempbyte & 0b1111);
        break;
        case 1:
        tempinst.operators[lopnum].tlevel = (tempbyte & 0b1111111);
        break;
        case 2:
        tempinst.operators[lopnum].keyscale = (tempbyte & 0b11000000) >> 6;
        tempinst.operators[lopnum].attack = (tempbyte & 0b11111);
        break;
        case 3:
        tempinst.operators[lopnum].enableam = (tempbyte & 0x80) >> 7;
        tempinst.operators[lopnum].decay = (tempbyte & 0b11111);
        break;
        case 4:
        tempinst.operators[lopnum].susrate = (tempbyte & 0b11111);
        break;
        case 5:
        tempinst.operators[lopnum].suslevel = (tempbyte & 0b11110000) >> 4;
        tempinst.operators[lopnum].release = (tempbyte & 0b1111);
        break;
        case 6:
        tempinst.operators[lopnum].ssg = (tempbyte & 0b1111);
        break;
      }
      j++;
    }
  }
  return tempinst;
}
