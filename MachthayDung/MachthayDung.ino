#include <Wire.h>
#define SENSOR 10
#define LED_ON 2 
#define RELAY 12 
#define LED 13

//Tu 00h00 den 2h00
#define HOUR_ON_1  0
#define MIN_ON_1   1
#define HOUR_OFF_1 1 
#define MIN_OFF_1  59
//Tu 2h00 den 5h30
#define HOUR_ON_2  2
#define MIN_ON_2   0
#define HOUR_OFF_2 5
#define MIN_OFF_2  30
/*
//Tu 00h00 den 2h00
#define HOUR_ON_1  0
#define MIN_ON_1   0
#define HOUR_OFF_1 1 
#define MIN_OFF_1  59
//Tu 2h00 den 5h30
#define HOUR_ON_2  2
#define MIN_ON_2   0
#define HOUR_OFF_2 5
#define MIN_OFF_2  30
*/
#define YES 1
#define NO  0

#define MODE0 0
#define MODE1 1
#define MODE2 2
#define MODE3 3
#define MODE4 4
 
const byte DS1307 = 0x68;
const byte NumberOfFields = 7;
 
int second, minute, hour, day, wday, month, year;
int sensorData;
int mode = 0;
int state;

void readDS1307(void);
int bcd2dec(byte num);
int dec2bcd(byte num);
void digitalClockDisplay(void);
void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr);
void turnOnRelay (void);
void turnOffRelay (void);
void checkTime (void);
void processSpeaker(int mode);
void systemProcess(void);
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(LED_ON, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(SENSOR,INPUT);
  //setTime(13, 06, 00, 2, 30, 10, 17); // 12:30:45 CN 08-02-2015
  turnOffRelay();
}
 
void loop()
{
  sensorData = digitalRead(SENSOR);
  Serial.print("Gia tri sensor: ");
  Serial.println(sensorData);
  readDS1307();
  checkTime();
  digitalClockDisplay();
  if(sensorData == YES){
    systemProcess();
  }
  else{
    state = 0;
  }
  digitalWrite(LED_ON,!digitalRead(LED_ON));
  delay(200);
}
 
void readDS1307(void){
    Wire.beginTransmission(DS1307);
    Wire.write((byte)0x00);
    Wire.endTransmission();
    Wire.requestFrom(DS1307, NumberOfFields);
    second = bcd2dec(Wire.read() & 0x7f);
    minute = bcd2dec(Wire.read() );
    hour   = bcd2dec(Wire.read() & 0x3f); // chế độ 24h.
    wday   = bcd2dec(Wire.read() );
    day    = bcd2dec(Wire.read() );
    month  = bcd2dec(Wire.read() );
    year   = bcd2dec(Wire.read() );
    year += 2000;    
}

int bcd2dec(byte num){
    return ((num/16 * 10) + (num % 16));
}

int dec2bcd(byte num){
    return ((num/10 * 16) + (num % 10));
}
 
void digitalClockDisplay(void){
    Serial.print(hour);
    printDigits(minute);
    printDigits(second);
    Serial.print(" ");
    Serial.print(day);
    Serial.print(" ");
    Serial.print(month);
    Serial.print(" ");
    Serial.print(year); 
    Serial.println(); 
}
 
void printDigits(int digits){
    Serial.print(":");
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}
 

void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr){
    Wire.beginTransmission(DS1307);
    Wire.write(byte(0x00));
    Wire.write(dec2bcd(sec));
    Wire.write(dec2bcd(min));
    Wire.write(dec2bcd(hr));
    Wire.write(dec2bcd(wd));
    Wire.write(dec2bcd(d)); 
    Wire.write(dec2bcd(mth));
    Wire.write(dec2bcd(yr));
    Wire.endTransmission();
}

void processSpeaker(int mode){
    int timeOn;
    /*
    if(mode == MODE0){
      timeOn = 50;
    }
    */
    if(mode == MODE1){
      timeOn = 100;
    }
    if(mode == MODE2){
      timeOn = 200;
    }
    if(mode == MODE3){
      timeOn = 200;
    }
    if(mode == MODE4){
      timeOn = 400;
    }
    digitalWrite(RELAY, HIGH);
    delay(timeOn);
    digitalWrite(RELAY, LOW);
    delay(timeOn);
    digitalWrite(RELAY, HIGH);
    delay(timeOn);
    digitalWrite(RELAY, LOW);
    delay(timeOn);
    digitalWrite(RELAY, HIGH);
    delay(timeOn);
    digitalWrite(RELAY, LOW);
}

void turnOnRelay (void){
    digitalWrite(RELAY, HIGH);
    digitalWrite(LED, HIGH);
}

void turnOffRelay (void){
    digitalWrite(RELAY, LOW);
    digitalWrite(LED, LOW);
}

void checkTime (void){
    int now = hour*100 + minute;
    int timeOnCheck_1 = HOUR_ON_1*100 + MIN_ON_1;
    int timeOffCheck_1 = HOUR_OFF_1*100 + MIN_OFF_1;
    
    int timeOnCheck_2 = HOUR_ON_2*100 + MIN_ON_2;
    int timeOffCheck_2 = HOUR_OFF_2*100 + MIN_OFF_2;
    mode = MODE0;
    if(now >= timeOnCheck_1 && now <= timeOffCheck_1){
        mode = MODE1;
    } 
    if(now >= timeOnCheck_2 && now <= timeOffCheck_2){
        mode = MODE3;
    }
}

void systemProcess(void){
    if(mode == MODE0){
        Serial.print("Mode: ");
        Serial.println(MODE0);
        digitalWrite(RELAY, HIGH);
        delay(25);
        digitalWrite(RELAY, LOW);
        delay(50);
        digitalWrite(RELAY, HIGH);
        delay(25);
        digitalWrite(RELAY, LOW);
        delay(10000);
        delay(10000);
        /*
        delay(10000);
        processSpeaker(MODE0);
        delay(10000);
        processSpeaker(MODE0);
        delay(10000);
        processSpeaker(MODE0);
        delay(5000);
        */
    }
    if(mode == MODE1 && state == 1){
        Serial.print("Mode: ");
        Serial.println(MODE2);
        Serial.print("State: ");
        Serial.println(state);
        processSpeaker(MODE2);
        delay(10000);
        processSpeaker(MODE2);
        delay(10000);
        processSpeaker(MODE2);
        delay(10000);
        processSpeaker(MODE2);
        delay(5000);
    }
    
    if(mode == MODE1 && state == 0){
        Serial.print("Mode: ");
        Serial.println(MODE1);
        Serial.print("State: ");
        Serial.println(state);
        processSpeaker(MODE1);
        delay(10000);
        processSpeaker(MODE1);
        delay(10000);
        processSpeaker(MODE1);
        delay(10000);
        processSpeaker(MODE1);
        delay(5000);
        state++;
    }
    
    if(mode == MODE3 && state == 1){
        Serial.print("Mode: ");
        Serial.println(MODE4);
        Serial.print("State: ");
        Serial.println(state);
        processSpeaker(MODE4);
        delay(10000);
        processSpeaker(MODE4);
        delay(10000);
        processSpeaker(MODE4);
        delay(10000);
        processSpeaker(MODE4);
        delay(5000);
    }
    
    if(mode == MODE3 && state == 0){
        Serial.print("Mode: ");
        Serial.println(MODE3);
        Serial.print("State: ");
        Serial.println(state);
        processSpeaker(MODE3);
        delay(10000);
        processSpeaker(MODE3);
        delay(10000);
        processSpeaker(MODE3);
        delay(10000);
        processSpeaker(MODE3);
        delay(5000);
        state++;
    }
    
}

