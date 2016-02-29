/* Serial 7-Segment Display Example Code
    SPI Mode Stopwatch
   by: Jim Lindblom
     SparkFun Electronics
   date: November 27, 2012
   license: This code is public domain.

   This example code shows how you could use the Arduino SPI 
   library to interface with a Serial 7-Segment Display.

   There are example functions for setting the display's
   brightness, decimals and clearing the display.

   The SPI.transfer() function is used to send a byte of the
   SPI wires. Notice that each SPI transfer(s) is prefaced by
   writing the SS pin LOW and closed by writing it HIGH.

   Each of the custom functions handle the ssPin writes as well
   as the SPI.transfer()'s.

   There's a custom function used to send a sequence of bytes
   over SPI - s7sSendStringSPI, which can be used somewhat like
   the serial print statements.

   Circuit:
   Arduino -------------- Serial 7-Segment
     5V   --------------------  VCC
     GND  --------------------  GND
      8   --------------------  SS
     11   --------------------  SDI
     13   --------------------  SCK
*/

struct ReturnDate  {
  int YEAR;
  int MONTH;
  int DAY; 
} ;


#include <SPI.h> // Include the Arduino SPI library
#include <Time.h>
#include <Wire.h>
#include <RTClib.h>
#include "ds3231.h"
#include "rtc_ds3231.h"

//RTC_DS3231 RTC;


// Define the SS pin
//  This is the only pin we can move around to any available
//  digital pin.
const int ssPin = 7;
const int ssPin2 = 10;
const int maxBuff = 20;

//int counter = 99;  // This variable will count up to 65k
char tempString[maxBuff];  // Will be used with sprintf to create strings
char datestring[maxBuff];
char inBuff[maxBuff];
char easter[] = "   Easter bunny    ";



unsigned long timeout = 20000L;
unsigned long nextTime = 0;
  DateTime DSTends;
  DateTime DSTbegins;


  int BaseDayNo, LastDayOfSchool;



DateTime GetDateTimeNow() {
  /*
  time_t t = now();
  DateTime dt = DateTime(year(t),month(t),day(t),hour(t),minute(t),second(t)) ;
  dt = DateTime( dt.unixtime() + 3600L );
  */
  struct ts t;
  DS3231_get(&t);
  DateTime dt = DateTime(t.year-108,t.mon,t.mday,t.hour,t.min,t.sec);
  return dt;
}


void setup()
{
  DS3231_init(DS3231_INTCN);
  BaseDayNo = GetJD(2015,8,19);
  LastDayOfSchool = GetJD(2016,6,2);

  Wire.begin();
  Serial.begin(9600);
  
  //RTC.begin();

  if ( timeStatus()!=timeSet ) { // ! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    //RTC.adjust(DateTime(__DATE__, __TIME__));
    DateTime dt = DateTime(__DATE__, __TIME__);
    /*time_t t = makeTime(dt);
    uint8_t _hour = dt.hour;
    int _minute=0;
    int _second=0; int _day = 0; int _year = 0;*/
    setTime(dt.hour(),dt.minute(),dt.second(),dt.day(),dt.month(),dt.year()); // CAUTION:
  }
  else
    Serial.println("Time is running.");

  DateTime now = GetDateTimeNow(); // RTC.now();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (now.unixtime() < compiled.unixtime()) {
    Serial.println("RTC is older than compile time!  Updating");
    /*
    RTC.adjust(DateTime(__DATE__, __TIME__));
    now = RTC.now();
    */
  } 
  else
    Serial.println("RTC time > complie time. No change to RTC."); 
  sprintf(tempString,"%02d:%02d:%02d",now.hour(),now.minute(),now.second());
  Serial.println(tempString);
  // -------- SPI initialization
  pinMode(ssPin, OUTPUT);  // Set the SS pin as an output
  pinMode(ssPin2,OUTPUT);
  digitalWrite(ssPin, HIGH);  // Set the SS pin HIGH
  digitalWrite(ssPin2,HIGH);
  SPI.begin();  // Begin SPI hardware
  SPI.setClockDivider(SPI_CLOCK_DIV64);  // Slow down SPI clock
  // --------

  // Clear the display, and then turn on all segments and decimals
  clearDisplaySPI(3);  // Clears display, resets cursor
  setDecimalsSPI(0,3);

  // Custom function to send four bytes via SPI
  //  The SPI.transfer function only allows sending of a single
  //  byte at a time.
  s7sSendStringSPI("-HI-",3);
  setDecimalsSPI(0b111111,3);  // Turn on all decimals, colon, apos

  // Flash brightness values at the beginning
  setBrightnessSPI(0,3);  // Lowest brightness

  _delay(1500);
  setBrightnessSPI(255,3);  // High brightness
  _delay(1500);
  s7sSendStringSPI("8888",3);
  _delay(2000);
  // Clear the display before jumping into loop
  clearDisplaySPI(3);
  setDecimalsSPI(0,3);  
  s7sSendStringSPI("   0",2);
  _delay(1000);
  scroll(easter,1);

  DSTends = DateTime(2015,11,1,2,0);
  DSTbegins = DateTime(2016,3,13,2,0);

  ShowDate(true,2);
  nextTime = millis()+timeout;
  printDays();
}
void scroll(char* s, int display) {
  for(int i=0;i<strlen(s)-4;i++) {
    strncpy(tempString,s+i,4);
    s7sSendStringSPI(tempString,display);
    _delay(500);
  }
}

bool blink = false;

unsigned long dateCount = 0;
bool alwaysscroll = false;
void loop()
{
  DateTime now = GetDateTimeNow(); //RTC.now();
  if(now.unixtime()>DSTends.unixtime() && now.unixtime()<DSTbegins.unixtime()) {
    // If not DST subtract an hour.
    now = DateTime( now.unixtime() - 3600L );
  }  
  sprintf(tempString, "%2d%02d", now.hour(),now.minute());
  s7sSendStringSPI(tempString,1);
  if (blink)
    setDecimalsSPI(0b00010000,1);  // Sets digit 3 decimal on
  else
    setDecimalsSPI(0,1);
  _delay(333);  // This will make the display update at 3Hz.
  blink = !blink;  
  if(millis()>nextTime) {
    dateCount++;
    if(dateCount%5==0 || alwaysscroll) {
      setDecimalsSPI(0,1);
      sprintf(datestring,"   %d-%d-%d     ",now.month(),now.day(),now.year());
      scroll(datestring,1);
      _delay(200);
      clearDisplaySPI(1);
      _delay(200);
      nextTime = millis()+timeout;
    } else {
      /*
      clearDisplaySPI(1);
      _delay(200);
      
      setDecimalsSPI(0b00000010,1);
      sprintf(tempString,"%2d%02d",now.month(),now.day());
      s7sSendStringSPI(tempString,1);  */
      ShowDate(true,1);
      _delay(1900);
      clearDisplaySPI(1);
      _delay(200);
      setDecimalsSPI(0,1);
      sprintf(tempString,"%d",now.year());
      s7sSendStringSPI(tempString,1);
      _delay(1900);
      clearDisplaySPI(1);
      _delay(200);
      nextTime = millis()+timeout;
    }
  }
  ShowDate(false, 2);
}    
void ShowDate(bool show, int display) {
  
  DateTime now = GetDateTimeNow(); // RTC.now();
  if(now.unixtime()>DSTends.unixtime() && now.unixtime()<DSTbegins.unixtime()) {
    // If not DST subtract an hour.
    now = DateTime( now.unixtime() - 3600L );
  }
  int tempF = 70; // (int)(RTC.getTempAsFloat()*9.0/5.0+32.0);
  int TheDaysLeft = GetDaysLeft( now.year(), now.month(), now.day(), now.hour(), now.minute() );
  //TheDaysLeft = tempF;
  if(TheDaysLeft>=0 && display>1) {
    sprintf(tempString,"%4d",TheDaysLeft);
    setDecimalsSPI(0,display);
    s7sSendStringSPI(tempString,display);
  }
  else {
    if(( /* now.hour()==0 && */ now.minute()==0 && now.second()==0) || show ) {
      clearDisplaySPI(display);
      setDecimalsSPI(0,display);
      delay(200);
      if(now.month()<10) {
        //strcpy(tempString,"    ");
        if(now.day()<10)
          sprintf(tempString,"%2d-%d",now.month(),now.day());  
        else
          sprintf(tempString,"%d-%d",now.month(),now.day());  
        setDecimalsSPI(0,display);    
      }
      else {
        sprintf(tempString,"%02d%02d",now.month(),now.day());
        setDecimalsSPI(0b00000010,display);
      }
      s7sSendStringSPI(tempString,display);
    }
  }
}
//time_t t;

byte DL2015_2016[] = {

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

void printDays() {
  int n = LastDayOfSchool - BaseDayNo;
  DateTime cur;
  DateTime Basedate = DateTime(2014,8,18,0,0);
  for(int i=0;i<=n+2;i++) {
    cur = DateTime(Basedate.unixtime() + i * 86400L);
    //GetGregorian(BaseDayNo+i,&yr,&mo,&dy);
    sprintf(tempString,"%3d %3d %5d %02d/%02d/%d", i, DL2015_2016[i], BaseDayNo+i, cur.month(), cur.day(), cur.year() );
    Serial.println(tempString);
  }
}

//                  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
byte DaysLeft[] = {20,19,18,18,18,17,16,15,14,13,13,13,12,11,10,9,  8, 8, 8, 7, 6, 5, 4, 3, 3, 3, 3, 2, 1, 0};
int GetDaysLeft(int year_, int month_, int day_, int hour_, int minute_ ) {
  int r = -1;
  int time;
  int JulianDayNo, OffsetDayNo; 
  JulianDayNo = GetJD(year_,month_,day_);
  /*
  if(year_==2014) {
    if( month_==5 ) {
      if(day_>0 && day_<=30)
        r = GetDaysLeft(day_-1);
        time = hour_*100+minute_;
        if(time>=1430)
          r=GetDaysLeft(day_);
    }
    if(month_>=8 && month_<=12 ) {
      //Serial.print("Base day no: "); Serial.print(BaseDayNo);
      //Serial.print(",   JDN: "); Serial.println(JulianDayNo);
      r = DL2015_2016[0];
      if(JulianDayNo>=BaseDayNo) {
        OffsetDayNo = JulianDayNo-BaseDayNo;
        if(OffsetDayNo>=0 && OffsetDayNo<=180 ) {
          r = DL2015_2016[OffsetDayNo];
          time = hour_*100+minute_;
          if(time>=1430)
            r = DL2015_2016[OffsetDayNo+1];
        }
        if(OffsetDayNo>180)
          r = 0;
      }    
    }
  }
  */
  if(year_>=2015) {
    /*
    int jd, y, m, d;
    GetGregorian(jd,&y,&m,&d); 
    */
    int maxArr = sizeof(DL2015_2016);
    if(JulianDayNo>=BaseDayNo) {
      OffsetDayNo = JulianDayNo-BaseDayNo;
      if(OffsetDayNo>=0 && OffsetDayNo<=maxArr ) {
        r = DL2015_2016[OffsetDayNo];
        time = hour_*100+minute_;
        if(time>=1440)
          r = DL2015_2016[OffsetDayNo+1];
      }
      if(OffsetDayNo>maxArr)
        r = 0;
    }
  }
  return r;
}
int GetDaysLeft(int pos) {
  int r = -1;
  if(pos>=0 && pos<30)
    r= DaysLeft[pos];
  return r;
}
/*
http://aa.usno.navy.mil/faq/docs/JD_Formula.php
*/

int GetJD( int YEAR, int MONTH, int DAY ) {
  int I = YEAR;
  int J = MONTH;
  int K = DAY;
  return  K-32075+1461*(I+4800+(J-14)/12)/4+367*(J-2-(J-14)/12*12)
          /12-3*((I+4900+(J-14)/12)/100)/4;
}


int GetGregorian( int JD, int* YEAR, int* MONTH, int* DAY ) {
  
  int I,J,K,L,N;

    L = JD+68569;
    N = 4*L/146097;
    L = L-(146097*N+3)/4;
    I = 4000*(L+1)/1461001;
    L = L-1461*I/4+31;
    J = 80*L/2447;
    K = L-2447*J/80;
    L = J/11;
    J = J+2-12*L;
    I = 100*(N-49)+I+L;

    *YEAR = I;
    *MONTH = J;
    *DAY = K;  
}


int charPos = 0;
void _delay(unsigned long ms) {
  int inByte;
  unsigned long WaitUntil = ms+millis();
  while(WaitUntil>millis()) {
    if(Serial.available()>0) {
      inByte = Serial.read();
      if(inByte==13 || inByte==10) {
        inBuff[charPos] = 0;
        if(charPos>0) {
          charPos = 0;
          sprintf(tempString,"%4s",inBuff);
          clearDisplaySPI(2);
          s7sSendStringSPI(tempString,2);
          delay(200);
        }
        charPos =0;
      } else if( inByte<32 ) {
        // do nothing
      } else {
        if(charPos<maxBuff) {
          inBuff[charPos] = inByte;
          charPos++;
        }
      }
    }
  }
}
// This custom function works somewhat like a serial.print.
//  You can send it an array of chars (string) and it'll print
//  the first 4 characters in the array.
void s7sSendStringSPI(String toSend, int display)
{
  if((display & 1) != 0 )
    digitalWrite(ssPin, LOW);
  if((display & 2) != 0 ) 
    digitalWrite(ssPin2,LOW);
  for (int i=0; i<4; i++)
  {
    SPI.transfer(toSend[i]);
  }
  digitalWrite(ssPin, HIGH);
  digitalWrite(ssPin2,HIGH);
}

// Send the clear display command (0x76)
//  This will clear the display and reset the cursor
void clearDisplaySPI(int display)
{
  if((display & 1) != 0 )
    digitalWrite(ssPin, LOW);
  if((display & 2) != 0 )
    digitalWrite(ssPin2,LOW);
  SPI.transfer(0x76);  // Clear display command
  digitalWrite(ssPin, HIGH);
  digitalWrite(ssPin2, HIGH);
}

// Set the displays brightness. Should receive byte with the value
//  to set the brightness to
//  dimmest------------->brightest
//     0--------127--------255
void setBrightnessSPI(byte value, int display)
{
  if((display & 1) != 0 ) {
    digitalWrite(ssPin, LOW);
    SPI.transfer(0x7A);  // Set brightness command byte
    SPI.transfer(255-value);  // brightness data byte
    digitalWrite(ssPin, HIGH);
  }
  if((display & 2) != 0 ) {
    digitalWrite(ssPin2,LOW);
    SPI.transfer(0x7A);  // Set brightness command byte
    SPI.transfer(value);  // brightness data byte
    digitalWrite(ssPin2, HIGH);  
  }
}

// Turn on any, none, or all of the decimals.
//  The six lowest bits in the decimals parameter sets a decimal 
//  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
//  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
void setDecimalsSPI(byte decimals, int display)
{
  if((display & 1) != 0 )
    digitalWrite(ssPin, LOW);
  if((display & 2) != 0 )
    digitalWrite(ssPin2,LOW);
  SPI.transfer(0x77);
  SPI.transfer(decimals);
  digitalWrite(ssPin, HIGH);
  digitalWrite(ssPin2, HIGH);    
}
