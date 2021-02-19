#include "Adafruit_GFX.h"     // Core graphics library
#include "MCUFRIEND_kbv.h"    // Hardware-specific library

MCUFRIEND_kbv tft;

#define BLACK 0x0000
#define RED 0xF800
#define GREEN 0x07E0
#define WHITE 0xFFFF
#define GREY 0x8410
#include "images.h"
#include "button_icons.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSerif12pt7b.h"
#include "FreeDefaultFonts.h"
#include <TimeLib.h>
#include <Wire.h>
#include <DS3231.h>
#include <Math.h>

DS3231 clock;

DS3231 timer;

RTCDateTime dt;

RTCDateTime timer_start;
RTCDateTime timer_current;

uint32_t start_timer_unixtime;
uint32_t now_timer_unixtime;

const int startTimerBtn = 53;
const int pauseTimerBtn = 51;

int a = 3000;
int b = 4000;
int j, j2;
int i, i2;

RTCDateTime t;

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg);

void setup()
{
  Serial.begin(9600);

  pinMode(startTimerBtn, INPUT_PULLUP);
  pinMode(pauseTimerBtn, INPUT_PULLUP);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.invertDisplay(true);
  tft.setRotation(1);

  
  tft.fillScreen(BLACK);
  tft.drawRGBBitmap(60, 120, pct1, 100, 40);
//  
  tft.drawRGBBitmap(60, 160, pct2, 100, 45);
  
  tft.drawRGBBitmap(210, 140, cimpress, 236, 45); 
  delay(4000); 
  
  tft.fillScreen(BLACK);
  
  // Initialize DS3231
  clock.begin();
  timer.begin();
}

char last[8];

struct Diff
{
  int hours;
  int minutes;
  int seconds;
};
Diff differenceBetweenTimePeriod(uint32_t start, uint32_t end1);

int ok = 0;

Diff difference;
void showTimer()
{
  if (ok == 1)
  {

    difference = differenceBetweenTimePeriod(start_timer_unixtime, now_timer_unixtime);
    char current_timer[20];
    sprintf(current_timer, "%02d:%02d", difference.minutes, difference.seconds);

    showmsgXY(250, 25, 3, NULL, current_timer);

    Serial.println(current_timer);
    timer_current = timer.getDateTime();
    now_timer_unixtime = timer_current.unixtime;
  }
}

void digitalClockDisplay(RTCDateTime dt)
{
  tft.setCursor(270, 210);
  tft.setFont();
  tft.setTextSize(6);
  tft.setTextColor(0xffff, 0x0000);

  char fullTime[6];
//  sprintf(fullTime, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
  sprintf(fullTime, "%02d:%02d", dt.hour, dt.minute, dt.second);
  tft.print(fullTime);
}

void digitalDayDisplay(RTCDateTime dt)
{
  tft.setTextSize(2);
  tft.setTextColor(0xffff);

  DS3231 ds3231 = DS3231();
  char fullDate[20];
  sprintf(fullDate, "%02d %s %02d", dt.day, ds3231.strMonth(dt.month), dt.year);

  char dayOfWeek[20];
  sprintf(dayOfWeek, "%s", ds3231.strDayOfWeek(dt.dayOfWeek));
  showmsgXY(330, 280, 1, &FreeSans12pt7b, dayOfWeek);
  showmsgXY(290, 305, 1, &FreeSans12pt7b, fullDate);
}

int startTimerBtnState = 0;
int pauseTimerBtnState = 0;
int resetPressCounter = 0;
int startTimerState = 0;
void loop(void)
{
   
  
  showmsgXY(25, 25, 3, NULL, "Logged time:");
  startTimerBtnState = digitalRead(startTimerBtn);
  pauseTimerBtnState = digitalRead(pauseTimerBtn);

  if (startTimerBtnState == LOW)
  {
    if (startTimerState == 0)
    {
      timer_start = timer.getDateTime();
      start_timer_unixtime = timer_start.unixtime;
      now_timer_unixtime = start_timer_unixtime;
      startTimerState = 1;
      showmsgXY(250, 25, 3, NULL, "00:00");
    }
    else
    {
      timer.setDateTime(now_timer_unixtime);
      start_timer_unixtime = timer_start.unixtime;
    }
    ok = 1;
    Serial.println("Pressed start");
    tft.drawRGBBitmap(350, 25, play_button, 24, 24); 
  }
  else
  {
    Serial.println("Unpressed start");
  }

  if (startTimerState == 0)
  {
    showmsgXY(250, 25, 3, NULL, "00:00");
    
    tft.drawRGBBitmap(350, 25, pause_button, 24, 24); 
  }
  
  if (pauseTimerBtnState == LOW)
  {
    Serial.println("Pressed pause");
    tft.drawRGBBitmap(350, 25, pause_button, 24, 24); 
    resetPressCounter++;
    ok = 0;
  }

  if (resetPressCounter >= 5)
  {
    startTimerState = 0;
    resetPressCounter = 0;
  }
  showTimer();

  dt = clock.getDateTime();

  

  showmsgXY(350, 150, 3, NULL, "21C");
  tft.drawBitmap(410, 140, Line7, 45, 45, 0xffff);  //rain

  digitalClockDisplay(dt);
  digitalDayDisplay(dt);
  
  tft.drawRGBBitmap(5, 275, cimpress, 236, 45); 
}

Diff differenceBetweenTimePeriod(uint32_t start, uint32_t end1)
{
  Diff diff;
  int diff_unixtime = end1 - start;
  diff.hours = diff_unixtime / 3600;
  diff.minutes = (diff_unixtime % 3600 / 60);
  diff.seconds = (diff_unixtime % 3600 % 60);
  return diff;
}

void showmsgXY(int x, int y, int sz, const GFXfont *f, const char *msg)
{
  y--;
  tft.setFont(f);
  tft.setCursor(x, y);
  tft.setTextColor(WHITE, 0x0000);
  tft.setTextSize(sz);
  tft.print(msg);
}
