float Heading[5] = {310 , 69 , 353, 96, 53}; float Heading_compass;

#include <Servo.h>
Servo servo1; //SG90
Servo servo2; // MG995

int IN1 = 9; int IN2 = 8; int IN3 = 7; int IN4 = 6; int enA = 10; int enB = 5; // 2 Motor 180 and L298N

#include "math.h"
#include <Wire.h>
#include <TinyGPSPlus.h> // GPS NEO-6M
TinyGPSPlus gps;
#include <SoftwareSerial.h>
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
SoftwareSerial ss(RXPin, TXPin);

// Count Waypoint
int j = 0; float lat_GPS,lon_GPS; float distance_Waypoint; float home_distance;

float Waypoint[5][2] = {
  {16.741615 , 107.155992}, // [0][0] and [0][1]
  {16.742073 , 107.156635}, // [1][0] and [1][1]
  {16.742663 , 107.157397}, // [2][0] and [2][1]
  {16.742602 , 107.156244}, // [3][0] and [3][1]
  {16.741744 , 107.155782}  // [4][0] and [4][1]  
};

// Ultrasoc Sensor
int trigger = A0; int echo = A1; float distance_L, distance_F, distance_R; int Set = 15; // SRF04
long Avoid(){int duration,distance;digitalWrite(trigger,HIGH);delay(10);digitalWrite(trigger,LOW);duration=pulseIn(echo,HIGH);distance=(duration/2)/29.1;return distance;}


void setup()
{
  Serial.begin(9600);ss.begin(GPSBaud);

  pinMode(trigger, OUTPUT); pinMode(echo, INPUT); 

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(enA, OUTPUT); pinMode(enB, OUTPUT); 

  servo1.attach(11);servo2.attach(12);servo1.write(90);
}

void loop()
{
  PC(); // Check data from PC


  // Data from GPS
  // getGPS(); 
  Serial.print('P'); Serial.println(lat_GPS); Serial.print('K'); Serial.println(lon_GPS); delay(100);  

  distance_Waypoint = distance(lat_GPS, lon_GPS, Waypoint[j][0], Waypoint[j][1]); Serial.print('W'); Serial.println(round(distance_Waypoint)); delay(100);
  home_distance = distance(lat_GPS, lon_GPS, Waypoint[0][0], Waypoint[0][1]); Serial.print('H'); Serial.println(round(home_distance)); delay(100);

  // Data from Compass
  getCompass();

  // Avoid obstacle
  distance_F = Avoid();

//  if (((distance_F > Set) || (distance_F < 0)) && (distance_Waypoint > 1)){ 
//    if ( abs(Heading_compass - Heading[j+1]) > 5 ){ Compass_Turn();}
//    else {Forword();}   
//  } 
//  else if ((distance_F < Set) && (distance_F > 0)) { Check_Go(); }
}

// Adjust Boat
void Compass_Turn(){
  Stop();
  getCompass(); 

  while ( abs(Heading_compass - Heading[j+1]) > 5 ){ // Chừng nào La bàn bị lệch hơn 5 độ         
    getCompass();                                                               
    PC();                                                                            
    Right();   
  }
  Stop();
}

// Check distance and Avoid obstacle
void Check_Go(){ 
  Stop(); delay(100); 
  for (int i = 90; i <= 180; i += 1){servo1.write(i); delay(20);} delay(1000); distance_R = Avoid();  // Check distance from Right side
  for (int i = 180; i >= 0; i -= 1){servo1.write(i); delay(20);} delay(1000); distance_L = Avoid();  // Check distance from Left side
  for (int i = 0; i <= 90; i += 1){servo1.write(i); delay(20);} delay(300); // Back to middle

  // Avoid obstacle
  if ((distance_L >= distance_R) && (distance_L > Set)){Left(); delay(500); Forword(); delay(600); Right(); delay(500); Forword(); delay(600); Right(); delay(400);} 
  else if ((distance_L < distance_R) && (distance_R > Set)){Right(); delay(500); Forword(); delay(600); Left(); delay(500); Forword(); delay(600); Left(); delay(400);} 
  else if ((distance_L < Set) && (distance_R < Set)){Backword(); delay(500); Left(); delay(500); Forword(); delay(600); Right(); delay(500); Forword(); delay(700); Right(); delay(400);} 
}

// GPS INFO
void getGPS(){
  while (ss.available() > 0) if (gps.encode(ss.read())) displayInfo();
  if (millis() > 5000 && gps.charsProcessed() < 10){Serial.println(F("No GPS detected: check wiring.")); while(true);}
}
void displayInfo(){
  if (gps.location.isValid()){lat_GPS = gps.location.lat(); lon_GPS = gps.location.lng();} else{ Serial.print(F("INVALID")); }
}

// Compass data
void getCompass(){
  if (Serial.available() > 0) { String val2 = Serial.readStringUntil('\n');
    if (val2.charAt(0)=='C') { val2.remove(0,1); Heading_compass = val2.toFloat(); }
  }
}

// Data from PC
void PC(){
  if (Serial.available() > 0){char val = char(Serial.read()); // Data from PC
  if(val == 'F') {Forword();}
  else if(val == 'B') {Backword();}
  else if(val == 'L') {Left();}
  else if(val == 'R') {Right();}
  else if(val == 'S') {Stop();}
  else if(val == '1') {servo2.write(180);} 
  else if(val == '2') {servo2.write(90);}}
} 

// Calculate distance from Waypoint
long double toRadians(const long double & degree){long double one_deg = (M_PI) / 180; return (one_deg * degree);}
long double distance(long double lat1, long double long1, long double lat2, long double long2){
  lat1 = toRadians(lat1); long1 = toRadians(long1); lat2 = toRadians(lat2); long2 = toRadians(long2);
  long double dlong = long2 - long1; long double dlat = lat2 - lat1;
  long double ans = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2); ans = 2 * asin(sqrt(ans));
  long double R = 3956;ans = ans * R;
  return ans;
}

// Contol motor
void Forword(){
  analogWrite(enA, 250); 
  analogWrite(enB, 250);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW); 
}
void Backword(){
  analogWrite(enA, 250); 
  analogWrite(enB, 250);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void Left(){
  analogWrite(enA, 200); 
  analogWrite(enB, 200);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void Right(){
  analogWrite(enA, 200); 
  analogWrite(enB, 200);  
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
void Stop(){
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
