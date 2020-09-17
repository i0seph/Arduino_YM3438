#ifndef YM3438_H
#define YM3438_H

#include <arduino.h>

// percent of math.log(1..128) value 
const float velocity_percent[128] PROGMEM = {0.0 ,14.285714285714285 ,22.642321438873665 ,28.57142857142857 ,33.17040135553375 ,36.92803572458795 ,40.105070315108634 ,42.857142857142854 ,45.28464287774733 ,47.45611564124804 ,49.42045169481854 ,51.21375001030224 ,52.86342454487275 ,54.39078460082292 ,55.81272279440741 ,57.14285714285714 ,58.39232630357628 ,59.57035716346161 ,60.684678763479795 ,61.741829926962325 ,62.7473917539823 ,63.70616598053282 ,64.62231365795733 ,65.49946429601651 ,66.3408027110675 ,67.14913883058703 ,67.92696431662098 ,68.6764988865372 ,69.39972850182247 ,70.0984370801217 ,70.7742330055268 ,71.42857142857143 ,72.06277313369219 ,72.67804058929057 ,73.27547167064237 ,73.8560714491759 ,74.42076236612786 ,74.97039304919409 ,75.50574598374641 ,76.02754421267662 ,76.53645720882977 ,77.03310603969659 ,77.51806792431569 ,77.99188026624711 ,78.45504423328107 ,78.90802794367163 ,79.35126930968053 ,79.7851785817308 ,80.21014063021727 ,80.62651699678179 ,81.03464774244993 ,81.43485311630133 ,81.82743506518857 ,82.21267860233527 ,82.59085305035228 ,82.9622131722515 ,83.32700020235346 ,83.68544278753674 ,84.03775784802632 ,84.38415136583598 ,84.72481910804123 ,85.05994729124107 ,85.38971319285595 ,85.71428571428571 ,86.0338259004065 ,86.34848741940647 ,86.6584170065396 ,86.96375487500487 ,87.264635096831 ,87.56118595635668 ,87.85353027863833 ,88.14178573489018 ,88.4260651268574 ,88.70647665184215 ,88.98312414994115 ,89.25610733490836 ,89.52552200992717 ,89.7914602694607 ,90.05401068824433 ,90.3132584983909 ,90.56928575549466 ,90.82217149454405 ,91.07199187638464 ,91.31882032541087 ,91.56272765911004 ,91.80378221002996 ,92.04204994069613 ,92.2775945519614 ,92.51047758523426 ,92.74075851899536 ,92.96849485998138 ,93.19374222938592 ,93.41655444440046 ,93.63698359539482 ,93.85508011901355 ,94.07089286744508 ,94.28446917410183 ,94.49585491593156 ,94.70509457256586 ,94.91223128249608 ,95.11730689645422 ,95.32036202816421 ,95.5214361026174 ,95.7205674020156 ,95.91779310951604 ,96.11314935090284 ,96.3066712343021 ,96.49839288804955 ,96.68834749681324 ,96.87656733606657 ,97.06308380500151 ,97.24792745796577 ,97.43112803450269 ,97.61271448806775 ,97.79271501349108 ,97.97115707325104 ,98.14806742262007 ,98.3234721337406 ,98.49739661868492 ,98.66986565155027 ,98.84090338963708 ,99.01053339375552 ,99.17877864770342 ,99.34566157695537 ,99.51120406660125 ,99.67542747857024 ,99.8383526681738 ,100.0};

  struct Operator {
    uint8_t detune; // 0..7, 0x30
    uint8_t multi; // 0..15, 0x30
    uint8_t tlevel; // 0..127, 0x40
    uint8_t keyscale; // 0..3, 0x50
    uint8_t attack; // 0..31, 0x50
    bool enableam; // 0x60
    uint8_t decay; // 0.31, 0x60
    uint8_t susrate; // 0.31, 0x70
    uint8_t suslevel; // 0..15, 0x80
    uint8_t release; // 0..15, 0x80
    uint8_t ssg; // 0..7=disable, 8..15, 0x90
  };

  struct Instrument {
    uint8_t feedback; // 0..7, 0xb0
    uint8_t algorithm; // 0..7, 0xb0
    bool enableLeft; // 0xb4
    bool enableRight; // 0xb4
    uint8_t amrate; // 0..3, 0xb4
    uint8_t fmrate; // 0..7, 0xb4
    Operator operators[4];
  };

class Ym3438 
{
  private:
    uint8_t pinIC, pinCS, pinWR, pinRD, pinA0, pinA1;
    uint8_t datapins[8];
    uint16_t fnum[12] = {617,   653,   692,   733,   777,   823,   872,   924,   979,  1037,  1099,  1164};
    uint8_t oparr[4] = {0,4,8,12};
    uint8_t inter_opnum[5] = {0,0,2,1,3};
  public:
    Ym3438(uint8_t cntrlpins[], uint8_t dpins[]);
    void setreg(uint8_t reg, uint8_t data1, bool enableA1);
    void getreg(uint8_t reg, uint8_t *data1, bool enableA1);
    void reset();

    void noteOff(uint8_t channel);
    void noteOn(uint8_t channel, uint8_t octave, uint8_t keynum);
    void noteOn(uint8_t channel, uint8_t octave, uint8_t keynum, Instrument inst, uint8_t velocity);
    
    void setLFO(bool isset, uint8_t hznum);
    void setLevel(uint8_t channel, uint8_t opnum, uint8_t vol);
    void setVelocity(uint8_t channel, Instrument inst, uint8_t vol); // 0..127

    // 0xb0
    void setFeedback(uint8_t channel, Instrument *inst, uint8_t value);
    void setAlgorithm(uint8_t channel, Instrument *inst, uint8_t value);
    // 0xb4
    void setLeft(uint8_t channel, Instrument *inst, bool value);
    void setRight(uint8_t channel, Instrument *inst, bool value);
    void setAMRate(uint8_t channel, Instrument *inst, uint8_t value);
    void setFMRate(uint8_t channel, Instrument *inst, uint8_t value);
    // operators
    void setDetune(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setMulti(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setKeyscale(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setAttack(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setAm(uint8_t channel, uint8_t opnum, Instrument *inst, bool value);
    void setDecay(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setSusrate(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setSuslevel(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setRelease(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    void setSsg(uint8_t channel, uint8_t opnum, Instrument *inst, uint8_t value);
    
    Instrument setInstrument(uint8_t channel, uint8_t instnum);
};

#endif // YM3438_H
