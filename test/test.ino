String val; float Heading_compass;

void setup()
{
  Serial.begin(9600);
}
void loop(){
  if (Serial.available() > 0) { 
    val = Serial.readStringUntil('\n');
    if (val.charAt(0)=='D'){ val.remove(0,1);
    Heading_compass = val.toFloat();
    Serial.println(Heading_compass); }
  }
}
