/*
 ENGR 111 Team 19
 This program calclates RPM's, power out, blade eff, motor eff, and total system eff for a hand constructed windmill that converts wind power to electrical power.
 */

//import library for LCD
#include<LiquidCrystal.h>

//init LCD
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);


//Pin Assignment for button
const int buttonPin = 3;
//pin assigment for motor
const int motorPin = A0; 

//declare global variables & constants for windmill calculations & display
//display settings var
int displaySetting = 1;
//max displays const
const int maxDisplays = 5;
//last display switch var
unsigned long lastDisplaySwitch = millis();
//display delay const
const int displayDelay = 250;
//rpm var
int rpm;
//doubles for windmill calculations
double power, bladeEff, sysEff, motorEff;


//setup function
void setup() {
  //Serial Setup
  Serial.begin(9600); // Intialize the serial monitor with a baude rate of 9600

  //LCD Setup
  lcd.begin(16, 2); // Initialize the LCD screen, which is 16 characters long and 2 characters tall
  //displays lcd
  lcd.display();
  //set button pin as input
  pinMode(buttonPin, INPUT);
  //calls changeDisplaySetting func when button is pressed
  attachInterrupt(digitalPinToInterrupt(buttonPin), changeDisplaySetting, FALLING);

  //Motor Input
  pinMode(motorPin, INPUT);
}

//loop function
void loop() {

  //get rpm input from PLC
  rpm = serial_comm();
  
  //calculate electrical power
  int voltageDrop = analogRead(A0);
  //double for resistor voltage and current
  double Vr, I;
  //resistence
  double R=10.0;
  //voltage across resistor
  Vr=(double)voltageDrop;
  //conversion needed for voltage across resistor
  Vr=Vr*5.0/1024.0;  
  //calc current across resistor
  I=Vr/R;
  //calc elec power
  power=Vr*I;

  //wind power
  double windPower;
  windPower = 23.85;
  
  //calculation for shaft power 
  //var for shaft power
  double shaftPower;
  //var for motor torque
  double motorTorque;
  //var for motor speed
  double motorSpeed;
  //vars for gear ratio
  //windmill gear number
  double Nw = 64.0;
  //motor gera number
  double Nm = 8.0;
  //var for windmill speed
  double windmillSpeed;
  //var for converted torque
  double convertTorque;
  //var for windmill torque
  double windmillTorque;
  //set windmill speed from plc
  windmillSpeed = (double)rpm;
  //calculate motor speed
  motorSpeed = (Nw/Nm)*windmillSpeed;
  //calculate motor torque
  motorTorque = (motorSpeed-12800)/(-40);
  //conversion for torque
  convertTorque = motorTorque*0.0000980665;
  //calculation for windmill torque
  windmillTorque = (convertTorque*motorSpeed)/windmillSpeed;
  //calculation for shaft power 
  shaftPower = ((windmillTorque*windmillSpeed)/9549.0)*1000;

  //blade eff calculation
  bladeEff = (shaftPower/windPower)*100;

  //motor eff calculation
  motorEff= (power/shaftPower)*100;

  //system eff calculation
  sysEff = (power/windPower)*100;


  //delay for one second
  delay(1000); 
  //display to LCD
  displayLCD();
  //delay for one second
  delay(1000);
}

//function that recieves input from plc and returns the input as rpm variable
int serial_comm() {
  while(Serial.available()>0);   // Do nothing until the serial input is received
  int rpm = Serial.parseInt(); // Convert the input to an integer and set equal to "rpm"
  return rpm;                   // Have the value of this function be set to the value of "rpm"
}

//function to display LCD depending on how many times the button is pressed
void displayLCD()
{
  //switch statement to display calc depending on which button was pressed
  switch(displaySetting)
  {
    //case 1 ie display settings equals 1
    case 1:
    //lcd clear
    lcd.clear();
    //print RPM
    lcd.print("RPM");
    //set curosr
    lcd.setCursor(0,1);
    //print rpm value
    lcd.print(rpm);
    //break out of switch statement 
    break;
    //case 2
    case 2:
    //clear lcd
    lcd.clear();
    //prints power
    lcd.print("POWER");
    //set cursor to 0 1
    lcd.setCursor(0,1);
    //prints power value to lcd
    lcd.print(power);
    //breaks out of switch
    break;
    //case 3
    case 3:
    // clear lcd
    lcd.clear();
    //prints blade eff to lcd
    lcd.print("BLADE EFF");
    //set cursor to 0 1
    lcd.setCursor(0,1);
    //prints out blade eff
    lcd.print(bladeEff);
    //breaks out of switch statement
    break;
    //case 4
    case 4:
    //clear lcd
    lcd.clear();
    //print motor eff
    lcd.print("MOTOR EFF");
    //set cursor to 0 1
    lcd.setCursor(0,1);
    //print motor eff value to lcd
    lcd.print(motorEff);
    //break out of switch statement
    break;
    //case 5
    case 5:
    //clear lcd
    lcd.clear();
    //print sys eff to lcd
    lcd.print("SYS EFF");
    //set lcd cursor to 0 1
    lcd.setCursor(0,1);
    //print sys eff value to lcd
    lcd.print(sysEff);
    //break out of switch statement
    break;
    //catch all if none of above conditions are met
    default:
    //clear lcd
    lcd.clear();
    //print unknown setting to lcd
    lcd.print("Unknown Setting!");
  }
}


//change lcd display settings when button is pressed and logic to cycle through calculations and then start at the beginning
void changeDisplaySetting() 
{
  if(lastDisplaySwitch + displayDelay < millis()) //This limits how quickly the LCD Display can switch
  {
    //sets last display switch var to millis func
    lastDisplaySwitch = millis();
    //prints switching display to serial
    Serial.println("Switching Display ...");
    //increment lcd settings
    displaySetting++;
    //toggles display settings back to 1 after toggled all the way through
    if(displaySetting > maxDisplays)
    {
      //sets display settings to 1
      displaySetting = 1;
    }
    //call to display lcd func
    displayLCD();
  }
}
