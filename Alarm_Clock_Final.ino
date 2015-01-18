#include <Time.h>

volatile float time = 0; //variable used to store the time in seconds
volatile float time_error = 0; // variable used to store the error in each overflow
volatile int state = LOW; // toggle state of the interrupt
int alarmHour = 0; //the hour of the alarm
int alarmMinute = 0;  // variable that stores the minute of the alarm
unsigned long startTime = 0;  // variable that stores the starttime for the snooze function
unsigned long endTime = 0;  // variable that stores the endtime for the snooze function
int increase = 6;  //the button wired to increase hours or minutes when setting alarm
int decrease = 7;  // the button used to decrease hours and minutes when setting alarm
int snooze = 3;  //the button used to snooze the alarm clock when pressed
unsigned long differenceSecs = 0;  //the variable used to store the difference between current time and alarm time
int alarmPin1 = 9;  // led output pin
volatile int check = 0;  
int alarmPin2 = 8;
int alarmPin3 = 5;
int inputPin = 0;
int count = 0;
int count2 = 0;
int speakerOut = 4;

void setup()
 {
   pinMode(increase, INPUT);
   pinMode(decrease, INPUT);
   pinMode(snooze, INPUT);
   pinMode(alarmPin1, OUTPUT);
   pinMode(alarmPin2, OUTPUT);
   pinMode(alarmPin3, OUTPUT);
   pinMode(speakerOut, OUTPUT);
   
  Serial.begin(9600);
  setTime(0,0,0,1,1,14);
  attachInterrupt(0,handler,RISING);
  TCCR0B = (_BV(CS01) | _BV(CS00));
  TCCR1A = 0;
  TIMSK1 |= _BV(TOIE1);
  TCCR1B = (_BV(CS11) | _BV(CS10));
  TCNT1 = 0;
 }
 
void loop()
{
  if(check == 1)
  {
    Serial.println("Adjust Alarm Hour");
    alarmHour = setAlarm();
    Serial.println("Adjust Alarm Minute");
    alarmMinute = setAlarm();
    differenceSecs = alarmHour*3600 + alarmMinute*60 - hour()*3600 - minute()*60 - second();
    startTime = timer();
    state = LOW;
  }  
  
    digitalClockDisplay();
    jmayDelay(1);
    
    if(count2 == 0 && startTime > 0)
    {
      endTime = timer();
      if(endTime - startTime >= differenceSecs)
      {
        triggerAlarm();
        count2++;
      }
    }
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void handler()
{  
  state=!state;
}

void printAlarm(int alarmHour, int alarmMinute)
{
  Serial.print(alarmHour);
  Serial.print(":");
  if(alarmMinute < 10)
  {
    Serial.print('0');
  }
  Serial.println(alarmMinute);
}
  
int setAlarm()
{
  int alarmTime = 0;
  
  while(state == HIGH)
  {
  if(count == 0)
  {
    printAlarm(alarmTime, alarmMinute);
  }
  else if(count == 1)
  {
    printAlarm(alarmHour, alarmTime);
  }
  
  while(digitalRead(increase) == HIGH || digitalRead(decrease) == HIGH)
  {}
  
  while(digitalRead(increase) == LOW && digitalRead(decrease) == LOW && state == HIGH);
  {}
 
  if(digitalRead(increase) == HIGH)
  {
    alarmTime = alarmTime + 1;
  }
  else if(digitalRead(decrease) == HIGH && alarmTime > 0)
  {
    alarmTime = alarmTime - 1;
  }
}
  
  state = HIGH;
  count++;
  return(alarmTime);
}

void triggerAlarm()
{
  while(state == LOW)
  {
    digitalWrite(alarmPin1, HIGH);
    digitalWrite(alarmPin2, HIGH);
    digitalWrite(alarmPin3, HIGH);
    tone(speakerOut, 500);
    jmayDelay(1);
    digitalWrite(alarmPin1, LOW);
    digitalWrite(alarmPin2, LOW);
    digitalWrite(alarmPin3, LOW);
    noTone(speakerOut);
    jmayDelay(1);
  }
  
  if (check == 2)
  {
    VronSnooze();
  }
  state = LOW;
  count = 0;
  count2 = 0;
  alarmMinute = 0;
  alarmHour = 0;
}

ISR(TIMER1_OVF_vect) 
{
  time += 0.262;
  time_error += 0.144;
  
  if(time_error >= 1000)
  {
    time_error -= 1000;
    time += 1;
  }
}

unsigned long timer()
{
  unsigned long t;
  t = time;
  return t;
}

void jmayDelay(int delayTime)
{
  int beginTime = timer();
  int nowTime = 0;
  while(nowTime - beginTime < delayTime)
  {
    nowTime = timer();
  }
}

void VronSnooze()
{
  int startTime2 = timer();
  int endTime2 = 0;
  while(endTime2 - startTime2 < 10)
  {
    endTime2 = timer();
    digitalClockDisplay();
    jmayDelay(1);
  }
  triggerAlarm();
}


