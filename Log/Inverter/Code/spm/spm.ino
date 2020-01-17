const int SpwmArry[] = {500,500,750,500,1250,500,2000,500,1250,500,750,500,500}; // Array of SPWM values.
const int SpwmArryValues = 13; //Put length of an Array depends on SpwmArray numbers. 
// Declare the output pins and choose PWM pins only
const int sPWMpin1 = 10;
const int sPWMpin2 = 9;
// enabling bool status of Spwm pins
bool sPWMpin1Status = true;
bool sPWMpin2Status = true;

void setup()
{
 pinMode(sPWMpin1, OUTPUT);
 pinMode(sPWMpin2, OUTPUT);
}

void loop()
{
 // Loop for Spwm pin 1
 for(int i(0); i != SpwmArryValues; i++)
 {
  if(sPWMpin1Status)
  {
   digitalWrite(sPWMpin1, HIGH);
   delayMicroseconds(SpwmArry[i]);
   sPWMpin1Status = false;
  }
  else
  {
   digitalWrite(sPWMpin1, LOW);
   delayMicroseconds(SpwmArry[i]);
   sPWMpin1Status = true;
  }
 }
 
 // Loop for Spwm pin 2 
 for(int i(0); i != SpwmArryValues; i++)
 {
  if(sPWMpin2Status)
  {
   digitalWrite(sPWMpin2, HIGH);
   delayMicroseconds(SpwmArry[i]);
   sPWMpin2Status = false;
  }
  else
  {
   digitalWrite(sPWMpin2, LOW);
   delayMicroseconds(SpwmArry[i]);
   sPWMpin2Status = true;
  }
 }
}
