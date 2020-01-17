#define MOSFET1  9
#define MOSFET2  10
//#define MOSFET3  11
//#define MOSFET4  12
#define NUM_SAMPLES 10
int sum = 0;                    // sum of samples taken
unsigned char sample_count = 0; // current sample number
float voltage = 0.0;            // calculated voltage
double IVoltage = 0;
double Current = 0;
float v = 0.0; 


void setup() {
  // put your setup code here, to run once:
  pinMode(MOSFET1, OUTPUT); // MOSFET 1
  pinMode(MOSFET2, OUTPUT);  // MOSFET 2
  pinMode(11,OUTPUT);
  setPwmFrequency(11, 8);
  analogWrite(11,127);
//  pinMode(MOSFET3, OUTPUT); 
//  pinMode(MOSFET4, OUTPUT); 
}

void loop() {
  //int pot=map(analogRead(A0),0,1023,10,50);
   //analogWrite(11,127);
  

      digitalWrite(MOSFET1, HIGH);   // MOSFET1 on 
      digitalWrite(MOSFET2, LOW);
      delay(7);  // wait for 8.3 mS

      
       digitalWrite(MOSFET1, LOW);   // MOSFET1 on 
      digitalWrite(MOSFET2, LOW);
      delay(10);  // wait for 8.3 mS
      
      digitalWrite(MOSFET1, LOW);  // MOSFET1 off
      digitalWrite(MOSFET2, HIGH);
      delay(7); 


      digitalWrite(MOSFET1, LOW);   // MOSFET1 on 
      digitalWrite(MOSFET2, LOW);
     delay(10);  // wait for 8.3 mS

  volt();

      
    }


void volt() {
 
        sum = analogRead(A0);
       
    // calculate the voltage
    // use 5.0 for a 5.0V ADC reference voltage
    // 5.015V is the calibrated reference voltage
    voltage = ((float)sum * 5.015) / 1024.0;
    // send voltage for display on Serial Monitor
    // voltage multiplied by 11 when using voltage divider that
    // divides by 11. 11.132 is the calibrated voltage divide
    // value
    v = voltage * 11.132;
    //Serial.println (v,2);
//    Serial.print ('\t');
//    Serial.println (a);
//    
    sample_count = 0;
    sum = 0;
   
   if(v>9)
 {
 digitalWrite(7,HIGH);  
 }
 else
 {
   digitalWrite(7,LOW);
 }
 }

    
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}   
 

