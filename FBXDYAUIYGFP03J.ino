#include <Wire.h>
#include <RTClibExtended.h>
#include <LowPower.h>
#include <TimeLord.h>
TimeLord tardis; 
void wakeUp();//This is the function performed on wake-up  Not really used in this program
#define wakePin 1    //use interrupt 3 (pin 1) and run function wakeUp when pin 1 gets LOW
#define ledPin 7   //use arduino on-board led for indicating sleep or wakeup status--not used to save power
#define songPin 6 //This pin goes low to start song on song board
#define pinRelayon 9 //These pins are used to toggle the relay
#define pinRelayoff 10
RTC_DS3231 RTC;      //we are using the DS3231 RTC
float const LONGITUDE = -156.000;//Change this location to suite wherever you hang your conch
float const LATITUDE = 20.000;//This is the location in South Maui


//-------------------------------------------------



//------------------------------------------------------------

void setup() {
  
  tardis.TimeZone(-10 * 60); // tell TimeLord what timezone your RTC is synchronized to. You can ignore DST
  // as long as the RTC never changes back and forth between DST and non-DST
  tardis.Position(LATITUDE, LONGITUDE); // tell TimeLord where in the world we are
   
  //Set pin D1 as INPUT for accepting the interrupt signal from DS3231
  pinMode(wakePin, INPUT_PULLUP);
  pinMode(songPin, OUTPUT);
  digitalWrite(songPin, HIGH);
  byte today[] = {  0, 0, 12, 7, 12, 2016    };
  //switch-on the on-board led for 1 second for indicating that the sketch is ok and running
  pinMode(ledPin, OUTPUT);//These are just to initialize the pins 
  pinMode(pinRelayon,OUTPUT);
  digitalWrite(pinRelayon, LOW);
  pinMode(pinRelayoff,OUTPUT);
  digitalWrite(pinRelayoff, LOW);
  delay(1000);
 
  //Initialize communication with the clock
  Wire.begin();
  RTC.begin();
  //Uncomment the following line out if you are setting your RTC for the first time...
  //RTC.adjust(DateTime(__DATE__, __TIME__));   //set RTC date and time to COMPILE time-
  //Serial.begin(9600);
  //clear any pending alarms
  RTC.armAlarm(1, false);
  RTC.clearAlarm(1);
  RTC.alarmInterrupt(1, false);
  RTC.armAlarm(2, false);
  RTC.clearAlarm(2);
  RTC.alarmInterrupt(2, false);
  RTC.writeSqwPinMode(DS3231_OFF);
  //Set SQW pin to OFF (in my case it was set by default to 1Hz)
  //The output of the DS3231 INT pin is connected to this pin
  //It must be connected to arduino D1 pin for wake-up
  
  digitalWrite(pinRelayoff, HIGH);//makes sure that relay is off initially
  delay(1000);
  digitalWrite(pinRelayoff,LOW);
  
}

//------------------------------------------------------------

void loop() {
   //This sets the alarm using TimeLord calculation and then shuts it down until the alarm goes off
    DateTime now = RTC.now();
    byte today[] = {  0, 0, 12, now.day(), now.month(), now.year()   }; // store today's date (at noon) in an array for TimeLord to use
    if (tardis.SunSet(today)) // if the sun will rise today (it might not, in the [ant]arctic)
   {
     RTC.setAlarm(ALM1_MATCH_HOURS, today[tl_minute], today[tl_hour], 0);   //set your wake-up time here 
     //digitalWrite(ledPin, HIGH); 
   }
   
    //RTC.setAlarm(ALM1_MATCH_HOURS,30, 18, 0);
    RTC.alarmInterrupt(1, true);
    attachInterrupt(3, wakeUp, FALLING);    //use interrupt 3 (pin 1) and run function wakeUp when pin 1 gets LOW                                     
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);    //Flora enters sleep mode here
    detachInterrupt(0); 
  
    //When exiting the sleep mode we clear the alarm
    RTC.armAlarm(1, false);
    RTC.clearAlarm(1);
    RTC.alarmInterrupt(1, false);
   //The following is the sequence for turning on the relay and firing the song button
  digitalWrite(ledPin,LOW);
  
  digitalWrite(pinRelayon,HIGH);
  delay(5000);
  digitalWrite(pinRelayon,LOW);
  delay(5000);
  digitalWrite(songPin, LOW);
  delay(60000);
  digitalWrite(songPin, HIGH);
  delay(3000);
  digitalWrite(pinRelayoff,HIGH);
  delay(1000);
  digitalWrite(pinRelayoff, LOW);
  delay(1000);

 
}
 
void wakeUp()
{
}
