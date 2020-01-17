
//Define ports
#define BUCK_ENABLE 7 
#define BUCK_PWM 6
#define VIN A3
#define IIN A2
#define VOUT A0
#define IOUT A1

//These voltages control the behaviour of our solar charger
#define ENABLE_VOLTAGE 18.0
#define DISABLE_VOLTAGE 15.0
#define INPUT_TARGET_VOLTAGE 17.0
#define OUTPUT_TARGET_VOLTAGE_LOW 13.8
#define OUTPUT_TARGET_VOLTAGE_HIGH 13.9
#define INPUT_CURRENT_LIMIT 2000.0
#define OUTPUT_CURRENT_LIMIT 3000.0
#define INPUT_CURRENT_MINIMUM 0.0

//Duty cycle limits
#define DUTY_CYCLE_MINIMUM 140
#define DUTY_CYCLE_MAXIMUM 240

//This compenstates for the timer running at a clock that is 64 faster than usual
#define DELAY_FACTOR 64

//The first term is a calculated or ideal multiplier
//The second term (the one close to 1.0) is an adjustment based on actual measurement
#define VIN_MULTIPLIER 0.023949 * 1.00507
#define VOUT_MULTIPLIER 0.01564 * 1.0036
#define IIN_MULTIPLIER 2.0798 * 0.99688
#define IOUT_MULTIPLIER 2.7153 *0.99202

//Set up the LCD library
#include <LiquidCrystal.h>
#define LCD_RS 12
#define LCD_RW 11
#define LCD_EN 10
#define LCD_D7 2
#define LCD_D6 3
#define LCD_D5 4
#define LCD_D4 5
LiquidCrystal lcd(LCD_RS, LCD_RW, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

//Some global variables
uint8_t buck_dutycycle;
float input_voltage;
float input_current;
float output_voltage;
float output_current;

//Set up the step-down DC-DC converter
void buck_setup()
{
  //Half-bridge driver enable signal
  pinMode(BUCK_ENABLE, OUTPUT);
  digitalWrite(BUCK_ENABLE, LOW);
  //Half-bridge driver PWM signal
  pinMode(BUCK_PWM, OUTPUT);
  //Fast PWM (mode 3) on OC0A (Arduino pin 6)
  TCCR0A = 0b00000011; 
  TCCR0B = 0b00000001; //Prescaler=1
  //Set dutycycle
  buck_dutycycle = 0;
  OCR0A = buck_dutycycle;    
}

//Turn on the step-down DC-DC converter 
void buck_enable()
{
  // Clock source enabled with prescaler=1
  TCCR0A &= 0b10111111;
  TCCR0A |= 0b10000000;
  //Enable signal high
  digitalWrite(BUCK_ENABLE, HIGH);
}

//Turn off the step-down DC-DC converter
void buck_disable()
{
  //Enable signal low
  digitalWrite(BUCK_ENABLE, LOW);
  // Clock source disabled
  TCCR0A &= 0b00111111;
  buck_dutycycle = 0;
}

//Set the duty-cycle for the DC-DC converter
void buck_duty(uint8_t duty)
{
  //The buck needs some off-time to keep the bootstrap capacitor of the upper FET charged
  //To ensure this we set a duty cycle clearly below 100%
  if(duty>DUTY_CYCLE_MAXIMUM)
  {
     duty = DUTY_CYCLE_MAXIMUM;
  }
  //A synchronous buck like ours can pump energy from the output to the input if the duty cycle drops below 50%
  //Eliminate this possibility by enforcing a duty cycle clearly above 50%
  if(duty<DUTY_CYCLE_MINIMUM)
  {
      duty = DUTY_CYCLE_MINIMUM;
  }
  buck_dutycycle = duty;
  OCR0A = duty;
}

//Shows measured voltages and currents on the LCD
void write_display()
{
  lcd.setCursor(0, 0);
  lcd.print("     V  ");
  lcd.setCursor(0, 0);
  lcd.print(input_voltage);
  
  lcd.setCursor(8, 0);
  lcd.print("     V  ");
  lcd.setCursor(8, 0);
  lcd.print(output_voltage);
  
  lcd.setCursor(0, 1);
  lcd.print("    mA  ");
  lcd.setCursor(0, 1);
  lcd.print((int16_t) input_current);
  
  lcd.setCursor(8, 1);
  lcd.print("    mA  ");
  lcd.setCursor(8, 1);
  lcd.print((int16_t) output_current);
}

//Measure input and output voltage and current
//Each value is measured 16 times and then averaged
void read_values()
{
  uint16_t vin_dac = 0;
  uint16_t iin_dac = 0;
  uint16_t vout_dac = 0;
  uint16_t iout_dac = 0;
  for(uint8_t j=0; j<16; ++j)
  {
    vin_dac += analogRead(VIN);
    vout_dac += analogRead(VOUT);
    iin_dac += analogRead(IIN);
    iout_dac += analogRead(IOUT);
  }
  input_voltage = vin_dac * VIN_MULTIPLIER / 16;
  output_voltage = vout_dac * VOUT_MULTIPLIER / 16;
  input_current = iin_dac * IIN_MULTIPLIER / 16;
  output_current = iout_dac * IOUT_MULTIPLIER / 16;
}

//Check if the duty cycle needs to be increased or decreased
//Also check if the DC-DC converter needs to be turned on or off
void buck_update()
{
  if(buck_dutycycle) //DC-DC converter is currently running
  {
    //Disable
    if((input_voltage<DISABLE_VOLTAGE) || (input_current<INPUT_CURRENT_MINIMUM))
    {
      buck_disable();
    }
    //Update
    else
    {
      //Reduce duty cycle
      if((input_voltage<INPUT_TARGET_VOLTAGE) || (output_voltage>OUTPUT_TARGET_VOLTAGE_HIGH) || (input_current>INPUT_CURRENT_LIMIT) || (output_current>OUTPUT_CURRENT_LIMIT))
      {
        buck_duty(buck_dutycycle-1);
      }
      //Increase duty cycle
      if((input_voltage>INPUT_TARGET_VOLTAGE) && (output_voltage<OUTPUT_TARGET_VOLTAGE_LOW)) 
      {
        buck_duty(buck_dutycycle+1);
      } 
    }
  }
  else //DC-DC converter is turned off
  {
    //Enable
    if(input_voltage>ENABLE_VOLTAGE)
    {
       buck_duty((uint8_t) (255*output_voltage/input_voltage));
       buck_enable();
    }
  }
}

void setup()
{
  //Show startup screen
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print(" Mathew & Ameer ");
  lcd.setCursor(0, 1);
  lcd.print(" ECE Dept ");
  
  //Set up DC-DC converter
  buck_setup();
  
  //Wait a bit to show the startup screen
  delay(500*DELAY_FACTOR);
}


//This function is called forever once setup() has completed
void loop()
{
  for(uint8_t i=0; i<32; ++i)
  {
    read_values();
    buck_update();
    delay(10*DELAY_FACTOR);
  }
  write_display();
}
