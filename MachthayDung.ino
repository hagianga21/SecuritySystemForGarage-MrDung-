#include <Wire.h> 
#define RELAY 12 
#define LED 13
#define HOUR_ON 12
#define MIN_ON 57
#define HOUR_OFF 13
#define MIN_OFF 10

const byte DS1307 = 0x68;
const byte NumberOfFields = 7;
 
int second, minute, hour, day, wday, month, year;
void readDS1307(void);
int bcd2dec(byte num);
int dec2bcd(byte num);
void digitalClockDisplay(void);
void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr);
void turnOnRelay (void);
void turnOffRelay (void);
void checkTime (void);
 
void setup()
{
  Wire.begin();
  Serial.begin(9600);
  pinMode(RELAY, OUTPUT);
  //setTime(13, 06, 00, 2, 30, 10, 17); // 12:30:45 CN 08-02-2015
  turnOffRelay();
}
 
void loop()
{
  readDS1307();
  checkTime();
  digitalClockDisplay();
  delay(1000);
}
 
void readDS1307(void)
{
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

int bcd2dec(byte num)
{
        return ((num/16 * 10) + (num % 16));
}

int dec2bcd(byte num)
{
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
 

void setTime(byte hr, byte min, byte sec, byte wd, byte d, byte mth, byte yr)
{
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
    int timeOnCheck = HOUR_ON*100 + MIN_ON;
    int timeOffCheck = HOUR_OFF*100 + MIN_OFF;
    
    if(now >= timeOnCheck && now < timeOffCheck){
        turnOnRelay();
    } 
    if(now >= timeOffCheck){
        turnOffRelay();
    }
}
