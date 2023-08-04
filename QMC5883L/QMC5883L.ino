#include <Wire.h> 
#define address 0x0D 
int xRaw, yRaw, zRaw;
int xMax = -32765;
int xMin = 32765;
int yMax = -32765;
int yMin = 32765;
struct MagnetometerSample{
  int X;
  int Y;
  int Z;
};
uint16_t  mode;
MagnetometerSample sample;
#define COMPASS_CONTINUOUS 0x00
#define COMPASS_SINGLE     0x01
#define COMPASS_IDLE       0x02
#define COMPASS_SCALE_088  0x00 << 2
#define COMPASS_SCALE_130  0x01 << 2
#define COMPASS_SCALE_190  0x02 << 2
#define COMPASS_SCALE_250  0x03 << 2
#define COMPASS_SCALE_400  0x04 << 2
#define COMPASS_SCALE_470  0x05 << 2
#define COMPASS_SCALE_560  0x06 << 2
#define COMPASS_SCALE_810  0x07 << 2
#define COMPASS_NORTH 0x00 
#define COMPASS_SOUTH 0x01
#define COMPASS_WEST  0x02
#define COMPASS_EAST  0x03
#define COMPASS_UP    0x04
#define COMPASS_DOWN  0x05
#define COMPASS_HORIZONTAL_X_NORTH  ( (COMPASS_NORTH << 6)  | (COMPASS_WEST  << 3)  | COMPASS_UP    ) << 5
#define COMPASS_HORIZONTAL_Y_NORTH  ( (COMPASS_EAST  << 6)  | (COMPASS_NORTH << 3)  | COMPASS_UP    ) << 5
#define COMPASS_VERTICAL_X_EAST     ( (COMPASS_EAST  << 6)  | (COMPASS_UP    << 3)  | COMPASS_SOUTH ) << 5
#define COMPASS_VERTICAL_Y_WEST     ( (COMPASS_UP    << 6)  | (COMPASS_WEST  << 3)  | COMPASS_SOUTH ) << 5

float declination_offset_radians=0;float Heading_compass;

int trigger2 = A2; int echo2 = A3; // JSN-SR04T
long Depth(){int duration,distance;digitalWrite(trigger2,HIGH);delay(10);digitalWrite(trigger2,LOW);duration=pulseIn(echo2,HIGH);distance=(duration/2)/29.1;return distance;}

void setup(){
  Serial.begin(9600);
  Wire.begin();Wire.beginTransmission(address); Wire.write(0x09); Wire.write(0x0D); Wire.endTransmission(); 
  Wire.beginTransmission(address); Wire.write(0x0B); Wire.write(0x01); Wire.endTransmission();

  pinMode(trigger2, OUTPUT); pinMode(echo2, INPUT);
}

void loop(){
  updateMagnetometerData(); sample.X = xRaw; sample.Y = yRaw; sample.Z = zRaw;
  Serial.print('C');Serial.println(GetHeadingDegrees(sample));delay(500);
  Serial.print('D'); Serial.println(Depth()); delay(500);
}

void updateMagnetometerData(){ Wire.beginTransmission(address); Wire.write(0x00); Wire.endTransmission(); Wire.requestFrom(address, 6);
  if(Wire.available()>=6){ xRaw = Wire.read(); xRaw |= Wire.read()<<8; yRaw = Wire.read(); yRaw |= Wire.read()<<8;; zRaw = Wire.read(); zRaw |= Wire.read()<<8; }
}

float GetHeadingDegrees(MagnetometerSample sample)
{  
  mode = COMPASS_SINGLE | COMPASS_SCALE_130 | COMPASS_HORIZONTAL_X_NORTH; float heading; float mag_north, mag_west;
  switch((mode >> 5) & 0x07 ){ case COMPASS_NORTH: mag_north = sample.Z; break; case COMPASS_SOUTH: mag_north = 0-sample.Z; break; 
    case COMPASS_WEST:  mag_west  = sample.Z; break; case COMPASS_EAST:  mag_west  = 0-sample.Z; break; case COMPASS_UP: case COMPASS_DOWN: break; }
  switch(((mode >> 5) >> 3) & 0x07 ){ case COMPASS_NORTH: mag_north = sample.Y;  break;case COMPASS_SOUTH: mag_north = 0-sample.Y; ;  break;
                                      case COMPASS_WEST:  mag_west  = sample.Y;  break; case COMPASS_EAST:  mag_west  = 0-sample.Y;  break; case COMPASS_UP: case COMPASS_DOWN: break; }
  switch(((mode >> 5) >> 6) & 0x07 ){ case COMPASS_NORTH: mag_north = sample.X; break; case COMPASS_SOUTH: mag_north = 0-sample.X; break;
                                      case COMPASS_WEST:  mag_west  = sample.X; break; case COMPASS_EAST:  mag_west  = 0-sample.X; break; case COMPASS_UP: case COMPASS_DOWN: break; }
  heading = atan2(mag_west, mag_north); heading += declination_offset_radians;
  if(heading < 0) heading += 2*M_PI;
  if(heading > 2*M_PI) heading -= 2*M_PI;

  return heading * 180/M_PI; 
}
