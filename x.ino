

/* FILE:    ARD_Multifunction_Shield_Seven_Segment_Example
   DATE:    28/05/14
   VERSION: 0.1

REVISIONS:

28/05/14 Created version 0.1

This is an example of how to use the 4 digit seven segment display on the Hobby
Components Arduino compatible Multi Function experimenter shield (HCARDU0085).

This example sketch will display a decimal number on the seven segment LED display
which will increment by one wever 100ms.

You may copy, alter and reuse this code in any way you like, but please leave
reference to HobbyComponents.com in your comments if you redistribute this code.
This software may not be used for the purpose of promoting or selling products
that directly compete with Hobby Components Ltd's own range of products.

THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR LACK OF NEGLIGENCE.
HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR ANY DAMAGES,
INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY
REASON WHATSOEVER.
*/
#include "Time.h"

/* Define shift register pins used for seven segment display */
#define LATCH_DIO 4
#define CLK_DIO 7
#define DATA_DIO 8


#define BUTTON1 A1
#define BUTTON2 A2
#define BUTTON3 A3

/*      1
 *     aaaa
 *    f    b  2
 * 32 f    b
 *     gggg      64
 *    e    c  4
 * 16 e    c
 *     dddd
 *      8
 *
 *  Segment byte maps for numbers 0 to 9
 *
 *  .gfedcda
 *  1
 *  2631
 *  84268421
                              0    1    2    3   4    5    6    7     8   9       */
const byte SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xD8, 0x80, 0x90};
/* Byte maps to select digit 1 to 4 */
const byte SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

const byte LED[] = {13, 12, 11, 10};

char buffer[5];

unsigned long Cur_ms_Count; // Stores the current time in ms
unsigned long Last_ms_Count; // Stores the last time in ms the counter was last updated
int Count; // Stores the value that will be displayed
bool showDP = true;

  int BaseDayNo, LastDayOfSchool;


void setup ()
{
  showDP = true;
   BaseDayNo = GetJD(2015,8,19);
  LastDayOfSchool = GetJD(2016,6,2);

  for (int i = 0; i < 4; i++) {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], HIGH);

  }
  /* Set DIO pins to outputs */
  pinMode(LATCH_DIO, OUTPUT);
  pinMode(CLK_DIO, OUTPUT);
  pinMode(DATA_DIO, OUTPUT);


  pinMode(BUTTON1, INPUT);
  digitalWrite(BUTTON1, HIGH);
  pinMode(BUTTON2, INPUT);
  digitalWrite(BUTTON2, HIGH);
  pinMode(BUTTON3, INPUT);
  digitalWrite(BUTTON3, HIGH);


  /* Initiliase the registers used to store the crrent time and count */
  Cur_ms_Count = millis();
  Last_ms_Count = 0;
  Count = 0;
  Serial.begin(9600);
  SetDateTime(__DATE__, __TIME__);
  //setTime(12,48,0,29,8,15);
}
/* sample input: date = "Dec 26 2009", time = "12:34:56"
 * jan feb mar apr may jun jul aug sep oct nov dec
 *
 * 
