void SetDateTime( char *date, char *time ) {
  int hr, min, sec;
  int month, day, year;
  month = 1;
  hr = 13; min = 18; sec = 0;
  hr = (time[0] - '0') * 10 + time[1] - '0';
  min = (time[3] - '0') * 10 + time[4] - '0';
  sec = (time[6] - '0') * 10 + time[7] - '0';

  char date4 = date[4];
  if (date4 == ' ')
    date4 = '0';
  day = (date4 - '0') * 10 + date[5] - '0';
  year = (date[9] - '0') * 10 + date[10] - '0';
  if (date[0] == 'D')
    month = 12;
  if (date[0] == 'J') {
    if (date[1] == 'a')
      month = 1;
    if (date[1] == 'u') {
      if (date[2] == 'l')
        month = 7;
      if (date[2] == 'n')
        month = 6;
    }
  }
  if (date[0] == 'N')
    month = 11;
  if (date[0] == 'O')
    month = 10;
  if (date[0] == 'F')
    month = 2;
  if (date[0] == 'M') {
    if (date[2] == 'r')
      month = 3;
    if (date[2] == 'y')
      month = 5;
  }
  if (date[0] == 'A')
  {
    if (date[1] == 'p')
      month = 4;
    if (date[1] == 'u')
      month = 8;
  }
  if (date[0] == 'S')
    month = 9;



  Serial.println("Time:");
  Serial.println(time);
  Serial.println("Date:");
  Serial.println(date);
  setTime(hr, min, sec, day, month, year);
}

int SetPosition = -1;
//int DaysLeft = 0;


/* Main program */
void loop()
{
  if (SetPosition >= 0) {
    if (!digitalRead(BUTTON1))  {
      if (buffer[SetPosition] < '9')
        buffer[SetPosition]++;
      while (!digitalRead(BUTTON1)) {
        delayWithLED(30);
        //delay(30);
      }
    }

    if (!digitalRead(BUTTON2))  {
      if (buffer[SetPosition] > '0')
        buffer[SetPosition]--;
      while (!digitalRead(BUTTON2)) {
        delayWithLED(30);
        //delay(30);
      }
    }
  }
  if (!digitalRead(BUTTON3)) {
    if (SetPosition < 0) {
      SetPosition = 0;
      sprintf(buffer, "%02d%02d", hour(), minute());
      delay(30);
      while (!digitalRead(BUTTON3)) {
        delayWithLED(30);
        //delay(30);
      }
    }
    else {
      /*
        SetPosition++;
        delay(30);
        */
      delay(30);
      SetPosition++;
      if (SetPosition > 3) {
        SetPosition = -1;
        SetClock();
      }
      while (!digitalRead(BUTTON3)) {
        delayWithLED(30);
        //delay(30);
      }
    }
  }
  else {
  }

  /*
  //* Get how much time has passed in milliseconds
  Cur_ms_Count = millis();
  //* If 100ms has passed then add one to the counter
  if(Cur_ms_Count - Last_ms_Count > 500)
  {
    Last_ms_Count = Cur_ms_Count;
    if(Count < 9999)
    {
      Count++;
    }
    else
    {
      Count = 0;
    }
  }
  //* Update the display with the current counter value
  WriteNumber2(Count);*/
  if (SetPosition < 0) {
    sprintf(buffer, "%2d%02d", hour(), minute());

    /*
    if( (millis() % 1000) >500 ) {
      digitalWrite(LED[3],LOW);
    }
    else {
      digitalWrite(LED[3],HIGH);
    }
    */
    unsigned long m = millis();
    m = (m / 100) & 7 ;
    set4(m);
    m = millis();

    showDP = true;
    if ( !digitalRead(BUTTON1) )  {
      sprintf(buffer, "%2d%02d", month(), day());
      if(!digitalRead(BUTTON2))
        doYear();
    }
    if ( !digitalRead(BUTTON2)  ) {
      sprintf(buffer, "%4d", GetDaysLeft( year(), month(), day(), hour(), minute() ));
      showDP = false;
      if(!digitalRead(BUTTON1) )
        doYear();
    }
  }
  WriteString(buffer);
}
void doYear() {
  showDP = false;
  sprintf(buffer,"%4d",year());
}
void delayWithLED(unsigned long ms) {
  unsigned long StopAt = millis() + ms;
  while (millis() < StopAt)
    WriteString(buffer);
}
void SetClock() {
  int hr = (buffer[0] - '0') * 10 + buffer[1] - '0';
  int min = (buffer[2] - '0') * 10 + buffer[3] - '0';
  if (hr > 0 && hr < 24 && min >= 0 && min < 60) {
    int sec = 0;

    int mo = month();
    int dy = day();
    int yr = year() % 100;
    setTime(hr, min, sec, dy, mo, yr);
  }
}
void set4( unsigned long v ) {
  digitalWrite(LED[3], v != 0 );
  digitalWrite(LED[2], v != 1 );
  digitalWrite(LED[1], v != 2 );
  digitalWrite(LED[0], v != 3 );
}
void WriteNumber2(int Number) {
  sprintf(buffer, "%4d", Number);
  WriteString(buffer);
}
void WriteString( char *s ) {
  int ch;
  if (strlen(s) == 4) {
    for (int i = 0; i < 4; i++) {
      ch = s[i] - '0';
      if (ch < 0 || ch > 9)
        ch = -1;
      if (i == SetPosition && ( millis() % 1000 ) > 500  )
        ch = -1;
      WriteNumberToSegment(i, ch);
    }
  }
}



/* Write a decimal number between 0 and 9999 to the display */
void WriteNumber(int Number)
{
  WriteNumberToSegment(0 , Number / 1000);
  WriteNumberToSegment(1 , (Number / 100) % 10);
  WriteNumberToSegment(2 , (Number / 10) % 10);
  WriteNumberToSegment(3 , Number % 10);
}

/* Wite a ecimal number between 0 and 9 to one of the 4 digits of the display */
void WriteNumberToSegment(byte Segment, int Value)
{
  digitalWrite(LATCH_DIO, LOW);
  int i = 0xFF;
  if (Value >= 0) {
    i = SEGMENT_MAP[Value];
  }
  if (Segment == 1 && showDP) {
    i = i & 0x7F;
  }

  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, i);
  shiftOut(DATA_DIO, CLK_DIO, MSBFIRST, SEGMENT_SELECT[Segment] );
  digitalWrite(LATCH_DIO, HIGH);
}
//                  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 
byte DaysLeft[] = {20,19,18,18,18,17,16,15,14,13,13,13,12,11,10,9,  8, 8, 8, 7, 6, 5, 4, 3, 3, 3, 3, 2, 1, 0};


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
