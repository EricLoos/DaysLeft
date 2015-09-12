
#include <Wire.h>
//#include "Chronodot.h"
#include <SPI.h>
#include <RTClib.h>
#include <RTC_DS3231.h>



//We always have to include the library
#include <Doomsday.h>

RTC_DS3231 RTC;
Doomsday doom;
//Chronodot RTC;

#define SUNDAY 0
#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6

#define PRAYER_DAY -1

/* we always wait a bit between updates of the display */
unsigned long delaytime=250;
unsigned long NextTime,NextGetTime,NextDots,NextAfterNine;

bool TwentyfourHourTime = true, DotsMode, ColonMode = true;
int DayTest = 1;
DateTime now;
int dots = 1;
int  buzzPin1 = A1; // This is the pin for the Buzzer
char *dows[] = {   "Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
//char *dows2[] = {  "Sunday","Monday","Tuesdy","Wedsdy","Thrsdy","Friday","Satrdy"};
char *love[] = {"I love","Judy","Loos"};
char buff[12];
char nBuff[5];

DateTime Basedate;

void setup() {
  dots = 0;
  Serial.begin(9600);
  Serial.println("Begin serial.");
  Serial.print("get length: ");
  Serial.println(GetLength());
  Wire.begin();
  doom.begin(72);
  //SetClock();
  //RTC.adjust(DateTime(2015,9,12,9,40,0));
  pinMode (buzzPin1, OUTPUT);
  beep(); 
  NextTime=millis()+2000;
  NextGetTime = millis()+300;
  NextDots = millis()+100;
  NextAfterNine = millis();
  DotsMode = false;
  DayTest = 0;
  
  now = RTC.now();
  Basedate = DateTime(2015,8,19,0,0);
  RTC.forceTempConv(true);
}

void beep() { /*
  if( !(dayOfWeek == SATURDAY || dayOfWeek == SUNDAY ) ) {
 tone(buzzPin1, 180, 300);
 delay(150);
 tone(buzzPin1, 200, 300);
 } */
}
int temp = 42;
int daysleft=180;
bool ShowDaysLeft = true;
unsigned long int mil = 0;
bool flag = false;
bool dir = true;
bool ToneDone = false;
bool PM = false;
int v;
//bool MilitaryTime = true;
bool TestWednesday = false;

void setTimeToBuff(char * s_) {
  if( TwentyfourHourTime ) {
    sprintf(buff,"%2d%s%02d%s%02d",now.hour(),s_,now.minute(),s_,now.second());
  }
  else {
    bool PM = false;
    int hr = now.hour();
    if(hr>=12)
      PM = true;
    if(hr>12)
      hr=hr-12;
    if(hr==0)
      hr=12;
    sprintf(buff,"%2d%s%02d%s%02d",hr,s_,now.minute(),s_,now.second());
    if(PM)
      strcat(buff,".");
  }
}
void padright(char* s, int n) {
  int ln = strlen(s);
  while(ln<n) {
    s[ln]=' ';
    s[ln+1]=0;
    ln = strlen(s);
  }
}
bool TenThirtyFifty( int n ) {
  /*
  return n==10 || n==11 ||
    n==30 || n==31 ||
    n==50 || n==51 ;
    */
  return (n / 10) % 2 != 0;
}
void loop() {
  //daysleft = GetDaysLeft();
  if(DayTest<3) {
    doom.printd(love[DayTest]);
    if(millis()<NextTime)
      return;
    NextTime=millis()+1000;
    DayTest++;
    return;
  }
  if(Serial.available()>0 ) {
    v = Serial.parseInt();
    Serial.println(v);
    if(v==555 ) {
      //SetClock();
      Serial.println("Clock set?");
    }
  }
  //GetTime();
  if(millis()>NextGetTime ) {
    now = RTC.now();
    NextGetTime = millis()+133;
  }
  daysleft = GetDaysLeft(now);
  //RTC.forceTempConv(true);
  /*
  int16_t temp_word = RTC.getTempAsWord();
  int8_t temp_hbyte = temp_word >> 8;
  int8_t temp_lbyte = temp_word &= 0x00FF;
  */
  float tempC = RTC.getTempAsFloat();
  float tempF = tempC*9/5+32.0;
  temp = (int)tempF; // now.tempF();
  Serial.print("RTC DOW: "); Serial.println(now.dayOfWeek());
  ShowDaysLeft = now.dayOfWeek() != PRAYER_DAY;  // SUNDAY;
  if(TestWednesday)
    ShowDaysLeft = false;
  /*
  if(hours==0 && minutes==0 && seconds==0) {
   hours=12; minutes=13; seconds=14;
   }
   */
  if(now.dayOfWeek() != PRAYER_DAY ) { // SUNDAY ) { // Not on wednesday
    if(now.hour()>=6 && now.hour()<=21) {
      if(now.hour()!=0 && (now.minute()==0 || now.minute()==30) && now.second()==0) {
        if(!ToneDone) {
          beep(); // tone(buzzPin1, 800, 200); 
          ToneDone = true;
        }
      }
      else
        ToneDone = false;
    }
    else
      ToneDone = false;
  }
  else {
    if(now.hour()>=6 && now.hour()<=18) {
      if(now.hour()!=0 && (now.minute()==0 || now.minute()==30) && now.second()==0) {
        if(!ToneDone) {
          beep(); // tone(buzzPin1, 800, 200); 
          ToneDone = true;
        }
      }
      else
        ToneDone = false;
    }
    else
      ToneDone = false;    
  }

  setTimeToBuff(":");
  //sprintf(buff,"%2d:%02d:%02d",now.hour(),now.minute(),now.second());
  mil = millis();
  if(mil>NextTime) {
    NextTime=millis()+2000;
    Serial.print("DOW: "); 
    Serial.print(now.dayOfWeek());
    Serial.print(" ( SUNDAY = "); 
    Serial.println(SUNDAY);       
    Serial.print("Twenty-four Hour time: "); 
    Serial.println(TwentyfourHourTime);
  }    
  if( ( now.dayOfWeek() == PRAYER_DAY /* && now.hour()>18 */ ) || TestWednesday )  /* || dayOfWeek == TUESDAY )  */ {
    if( now.hour() != 18 ) { // Skip 6PM.
      if(now.minute()>=25 && now.minute()<30 ) {
        if(mil % 500 > 200 ) {
          //sprintf(buff,"%2d:%02d:%02d",now.hour(),now.minute(),now.second());
          setTimeToBuff(":");
          //dots=0;
        }
        else {
          strcpy(buff,"  :  :  ");
        }
        //dots=0b111111111111;
        if( millis()>NextDots) {
          NextDots = millis()+200;
          if(DotsMode) {
            dots = 0b101010101010;
          }
          else {
            dots = 0b010101010101;
          }
          DotsMode = !DotsMode;
        }
      }
      else if(now.minute()>=15 && now.minute()<25) {
        if( mil % 800 > 400 ) {
          //sprintf(buff,"%2d:%02d:%02d",now.hour(),now.minute(),now.second());
          setTimeToBuff(":");
        }
        else {
          //sprintf(buff,"%2d%02d%02d",now.hour(),now.minute(),now.second());
          setTimeToBuff("");
        }
        if(mil % 200 > 100 ) {
          if(!flag) {
            if(dir)
              dots = dots << 1;
            else
              dots = dots >> 1;
            if(dots>0b111111111111) {
              dots = 0b10000000000;
              dir = false;
            }
            if(dots<=1 ) {
              dots = 1;
              dir = true;
            }
            flag = true;
          }
        }
        else {
          flag = false;
        }
      }
      else
        dots = 0;
    }
    else 
      dots=0;
  }
  else {
    //dots = 0;
    if(ShowDaysLeft) {
          sprintf(buff,"%s%3d",nBuff,temp);
        }
      }
      if(  now.second() % 10 <= 1 && now.second()>=10  ) {
        dots = 0;
        sprintf(buff,"%s%3d",dows[now.dayOfWeek()],temp);
        if(TenThirtyFifty(now.second())) {
          sprintf(nBuff,"%d",daysleft);
          padright(nBuff,3);
      if(now.second()<=2) {
        //dots=0b111111111111;
        if( millis()>NextDots) {
          NextDots = millis()+200;
          if(DotsMode) {
            dots = 0b101010101010;
          }
          else {
            dots = 0b010101010101;
          }
          DotsMode = !DotsMode;
        }
        sprintf(buff,"%2d'%02d'%02d",now.month(),now.day(),now.year() % 100);
      }
      else
        dots = 0;
    }
  }

  if(now.hour()>=22 || now.hour()<5 ) {
    dots=0;
    if(now.hour()>=12 && !TwentyfourHourTime )
      dots = 0b100000000000;
    //setTimeToBuff(":");
    if(ColonMode) {
      setTimeToBuff(":");
    } else {
      setTimeToBuff("");
    }
    
    if(millis()>NextAfterNine) {
      ColonMode = !ColonMode;
      if(ColonMode)
        NextAfterNine = millis()+3000;
      else
        NextAfterNine = millis()+50;
    }
    
    //setTimeToBuff(":");
  }
  doom.printd(buff,dots);
} 
//                    0              1    2          3           4         5           6             7        8   9
byte segments[] = {
  64+32+16+8+4+2,32+16,64+32+1+4+8,64+32+16+8+1,32+16+1+2,64+2+1+16+8,64+2+1+16+8+4,64+32+16,127,64+32+16+8+1+2};

void printLED(char *s )
{
  /*
  char ch; //, ch1;
   byte bits;
   byte flag;
   for(int i=0;i<strlen(s);i++) {
   //ch1 = s[i];
   ch = s[i]; // toupper(ch1);
   switch( ch ) {
   case 'R':
   lc.setRow(0,7-i,0x05);
   break;        
   case 'U':
   lc.setRow(0,7-i,0x1c);
   break;         
   case 'I':
   lc.setRow(0,7-i,B00010000);
   break;
   case 'N':  
   lc.setRow(0,7-i,0x15);
   break;
   case 'O':
   lc.setRow(0,7-i,0x1D);
   break;
   case 127:
   lc.setRow(0,7-i,1+2+32+64);
   break;
   default:   
   flag = (ch & 128) != 0;
   ch = ch & 127;
   lc.setChar(0,7-i,ch,flag);
   break;
   }
   } */
  doom.printd(s);
}

/*
  This method will scroll all the hexa-decimal
 numbers and letters on the display. You will need at least
 four 7-Segment digits. otherwise it won't really look that good.
 */
void scrollDigits() {
  for(int i=-8;i<16;i++) {
    for(int j=0;j<8;j++) {/*
      if(i+j>15 || i+j<0)
     lc.setChar(0,7-j,' ', false);
     else
     lc.setDigit(0,7-j,i+j,false);*/
    }
    /*
    lc.setDigit(0,7,i,false);
     lc.setDigit(0,6,i+1,false);
     lc.setDigit(0,5,i+2,false);
     lc.setDigit(0,4,i+3,false);
     
     lc.setDigit(0,3,i+4,false);
     lc.setDigit(0,2,i+5,false);
     lc.setDigit(0,1,i+6,false);
     lc.setDigit(0,0,i+7,false);
     */
    delay(delaytime);
  }
  //lc.clearDisplay(0);
  delay(delaytime);
}
/*
void loop() { 
 writeArduinoOn7Segment();
 //scrollDigits();
 }
 */
// dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
//char buff[12];
void printLEDf( float f, int width, int places ) {
  boolean dp = false;
  int offset=0;
  dtostrf(f,width+1,places,buff);
  //printLED(buff);
  for(int i=0;i<8;i++) {
    if(buff[i+1]=='.')
      dp=true;
    //lc.setChar(0,7-i,buff[i+offset],dp);
    if(buff[i+1]=='.')
      offset=1;
    dp=false;
  } 
}

byte bcdToDec(byte val)
{
  return ( (val/16*10) + (val%16) );
}

/*
void GetTime() {
  // send request to receive data starting at register 0
  Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
  Wire.write((byte)0); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(0x68, 7); // request three bytes (seconds, minutes, hours)

  while(Wire.available())
  { 
    seconds = bcdToDec(Wire.read()); // get seconds
    minutes = bcdToDec(Wire.read()); // get minutes
    hours = bcdToDec(Wire.read());   // get hours
    dayOfWeek  = bcdToDec(Wire.read());
    dayOfMonth = bcdToDec(Wire.read());
    month      = bcdToDec(Wire.read());
    year       = bcdToDec(Wire.read());

    
//    seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
//     minutes = (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal
//     hours = (((hours & 0b00100000)>>5)*20 + ((hours & 0b00010000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
     s
  }
}
*/
byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}
/*
void SetClock() {
  Wire.beginTransmission(0x68);  // Open I2C line in write mode

  Wire.write((byte)0x00);                           // Set the register pointer to (0x00)
  Wire.write(decToBcd(0));               // Write seven bytes
  Wire.write(decToBcd(26)); // minute
  Wire.write(decToBcd(19));   // hour   
  Wire.write(decToBcd(3)); // DOW
  Wire.write(decToBcd(18)); // day of month
  Wire.write(decToBcd(5)); // month
  Wire.write(decToBcd(15)); // year
  Wire.endTransmission();      
}*/

void FixBuff() {
  bool OneDotFound = false;
  int len = strlen(buff);
  while(true) {
    OneDotFound = false;
    for(int i=0; i<len; i++) {
      if(buff[i+1]=='.') {
        buff[i]=buff[i] | 128;
        OneDotFound=true;
        for(int j=i+1;j<len+1;j++){
          buff[j]=buff[j+1];
        }
        len--;
      }
    }
    if(!OneDotFound)
      break;
  }
}

//byte DL2014_2015[] = {
const byte DL2015_2016[] PROGMEM = {
  180,179,178,177,177,177,176,175,174,173,172,172,172,171,170,
  169,168,167,167,167,167,166,165,164,163,163,163,162,161,160,
  159,158,158,158,157,156,155,154,153,153,153,152,151,150,149,
  148,148,148,147,146,145,144,143,143,143,142,141,140,139,138,
  138,138,137,136,135,134,133,133,133,132,131,130,129,128,128,
  128,127,126,125,124,123,123,123,122,121,121,120,119,119,119,
  118,117,116,115,114,114,114,114,114,114,114,114,114,114,113,
  112,111,110,109,109,109,108,107,106,105,104,104,104,103,102,
  101,100,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,
  98,97,96,95,94,94,94,93,92,91,90,89,89,89,89,88,87,86,85,85,
  85,84,83,82,81,80,80,80,79,78,77,76,75,75,75,75,74,73,72,71,
  71,71,71,70,69,68,67,67,67,66,65,64,63,62,62,62,61,60,59,58,
  57,57,57,56,55,54,53,52,52,52,51,50,49,48,47,47,47,47,47,47,
  47,47,47,47,47,46,45,44,43,43,43,42,41,40,39,38,38,38,37,36,
  35,34,33,33,33,32,31,30,29,28,28,28,27,26,25,24,23,23,23,22,
  21,20,19,18,18,18,17,16,15,14,13,13,13,12,11,10,9,8,8,8,7,6,
  5,4,3,3,3,3,2,1,0
};

int GetLength() {
  return sizeof(DL2015_2016)/sizeof(byte);
}
/*
int GetDaysLeft(int year_, int month_, int day_, int hour_, int minute_) {
  int time = hour_*100+minute_;
  return  pgm_read_byte(&DL2015_2016[0]);
}
*/

int GetDaysLeft( DateTime n ) {
  int r = pgm_read_byte(&DL2015_2016[0]);
  int time = n.hour()*100+n.minute();
  int diff = 0;
  if(n.unixtime() >= Basedate.unixtime() ) {
    diff = ( n.unixtime() - Basedate.unixtime() ) / 86400L ;
    if(diff>=0 && diff<GetLength() ) {
      r=pgm_read_byte(&DL2015_2016[diff]);
      if(time>=1440)
        r = pgm_read_byte(&DL2015_2016[diff+1]);
    }
  }
  return r;
}
