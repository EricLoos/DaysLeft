#include <Wire.h>

//We always have to include the library
#include <Doomsday.h>

Doomsday doom;

#define SUNDAY 0
#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6

//DS3231 Clock;
/* we always wait a bit between updates of the display */
unsigned long delaytime=250;

 int seconds;
int minutes;
int hours;

int dayOfWeek ;
int dayOfMonth;
int month     ;
int year      ;

int  buzzPin1 = A1; // This is the pin for the Buzzer

void setup() {
  Serial.begin(9600);
  Serial.println("Begin serial.");
  Wire.begin();
  doom.begin(72);
  //SetClock();
  pinMode (buzzPin1, OUTPUT);
  beep(); 
}

void beep() {
  tone(buzzPin1, 800, 200);
}
int daysleft = 53;
bool ShowDaysLeft = true;

char buff[12];
/*
 This method will display the characters for the
 word "Arduino" one after the other on digit 0. 
 */
int dots = 1;
unsigned long int mil = 0;
bool flag = false;
bool dir = true;
bool ToneDone = false;
bool PM = false;
int v;
void loop() {
  if(Serial.available()>0 ) {
    v = Serial.parseInt();
    Serial.println(v);
    if(v==555 ) {
      SetClock();
      Serial.println("Clock set");
    }
  }
  GetTime();
  ShowDaysLeft = dayOfWeek != WEDNESDAY;
  /*
  if(hours==0 && minutes==0 && seconds==0) {
    hours=12; minutes=13; seconds=14;
  }
  */
  if(dayOfWeek != WEDNESDAY) { // Not on wednesday
    if(hours>=6 && hours<=21) {
      if(hours!=0 && (minutes==0 || minutes==30) && seconds==0) {
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
    if(hours>=6 && hours<=18) {
      if(hours!=0 && (minutes==0 || minutes==30) && seconds==0) {
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

    PM = false;
    if(hours>=12) {
      PM = true;
    }
    if(hours>12)
      hours = hours - 12;
    if(hours==0)
      hours = 12;
    sprintf(buff,"%2d:%02d:%02d",hours,minutes,seconds);
    if(PM)
      strcat(buff,".");
    mil = millis();
    /*
    if(seconds == 45) {
      dots = 0;
      sprintf(buff,"%6d",dayOfWeek);
    }
    else */
    if(dayOfWeek == WEDNESDAY )  {
      if(minutes>=25 && minutes<30 ) {
        if(mil % 500 > 200 ) {
          sprintf(buff,"%2d:%02d:%02d",hours,minutes,seconds);
          dots=0;
        }
        else {
          strcpy(buff,"  :  :  ");
          dots=0b111111111111;
        }
      }
      else if(minutes>=15 && minutes<25) {
        if( mil % 800 > 400 ) {
          sprintf(buff,"%2d:%02d:%02d",hours,minutes,seconds);
        }
        else {
          sprintf(buff,"%2d%02d%02d",hours,minutes,seconds);
        }
        if(PM)
          strcat(buff,".");
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
  else {
    dots = 0;
    if(ShowDaysLeft) {
      if(  seconds % 10 <= 1 && seconds>=10  ) {
        sprintf(buff,"%6d",daysleft);
      }
    }
  }
  /*
  if(mil % 1000 == 0 ) {
    dots = dots * 2;
  }
  
    if((dots & 0b111111111111) == 0 )
      dots = 1;
  */
  //dots = 1 << (seconds/5) ;
  doom.printd(buff,dots);
} 
//                    0              1    2          3           4         5           6             7        8   9
byte segments[] = {64+32+16+8+4+2,32+16,64+32+1+4+8,64+32+16+8+1,32+16+1+2,64+2+1+16+8,64+2+1+16+8+4,64+32+16,127,64+32+16+8+1+2};

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
       
 /*
    seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
    minutes = (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal
    hours = (((hours & 0b00100000)>>5)*20 + ((hours & 0b00010000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
    */
  }
}

byte decToBcd(byte val)
{
  return ( (val/10*16) + (val%10) );
}

void SetClock() {
  Wire.beginTransmission(0x68);  // Open I2C line in write mode
 
   Wire.write((byte)0x00);                           // Set the register pointer to (0x00)
   Wire.write(decToBcd(0));               // Write seven bytes
   Wire.write(decToBcd(8)); // minute
   Wire.write(decToBcd(22));   // hour   
   Wire.write(decToBcd(3)); // DOW
   Wire.write(decToBcd(6)); // day of month

   Wire.write(decToBcd(3)); // month
   Wire.write(decToBcd(13)); // year
   Wire.endTransmission();      
}

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
