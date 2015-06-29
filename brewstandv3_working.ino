/* Strike, Mash, and Boil Sensors with LED and sound indicators
 ADDED TEST NOTE

 DS18B20 Pinout (Left to Right, pins down, flat side toward you)
 - Left   = Ground
 - Center = Signal (with 3.3K to 4.7K resistor to +5 or 3.3 )
 - Right  = +5 or +3.3 V
 LCD Pinout
 Pin 1 to Arduino GND
 Pin 2 to Arduino 5V
 Pin 3 to wiper ( this is the middle pin of the 10k potentiometer )
 Pin 4 (RS) to Arduino pin 7
 Pin 5 (RW) to Arduino GND
 Pin 6  (EN) to Arduino pin 8
 Pin 11 (DB4) to Arduino pin 9
 Pin 12 (DB5)to Arduino  pin 10
 Pin 13 (DB6) to Arduino pin 11
 Pin 14 (DB7) to Arduino pin 12
 Pin 15 to 5V (with resistor?)
 Pin 16 to GND
 Arduino Pinout
 D2 to _
 D3 to _
 D4 to _
 D5 to DS18B20 Signal
 D6 to Piezo/Buzzer HIGH
 D7 to LCD Pin 4
 D8 to LCD Pin 6
 D9 to LCD Pin 11
 D10 to LCD Pin 12
 D11 to LCD Pin 13
 D12 to LCD Pin 14
 D13 to Boil LED
 A0 to Strike Blue LED
 A1 to Strike Green LED
 A2 to Strike Red LED
 A3 to Mash Blue LED 
 A4 to Mash Green LED
 A5 to Mash Red LED
 A6 to _
 */

#include <OneWire.h>
#include <DallasTemperature.h>
//#include <Wire.h>
#include <LiquidCrystal.h>


/*-----( Declare Constants and Pin Numbers )-----*/
#define ONE_WIRE_BUS 5

/***** The below 4 INT can be changed to suit your system.
       Strike water temp is auto calculated *****/
int T1 = 70; //Grain temp
int mashTemp = 151; //Temp you want to mash at
int r = 1.5; //this is my typical ratio grain to water
int boilTemp = 204; //boiling temp for your elevation

/*
Strike temp calculation John Palmer: ***using this one currently
Tw = (.2/r)(T2 - T1) + T2

Strike temp from brew365.com references below
Tw =((((G*0.05)+SV)*T2)-((G*0.05)*G2))/SV

r = The ratio of water to grain in quarts per pound.
T1 = The initial temperature (¡F) of the mash (or grains).
T2 = The target temperature (¡F) of the mash.
Tw = The actual temperature (¡F) of the infusion water.

*/

int strikeTemp;

//LED setup below
//LED 1 approaching STRIKE
const int bluePin1 = A0;
const int greenPin1 = A1;
const int redPin1 = A2;
int myCounter1 = 0;
// LED 2 approaching MASH
const int bluePin2 = A3;
const int greenPin2 = A4;
const int redPin2 = A5;
int myCounter2 = 0;
// LED 3 approaching BOIL
const int redPin3 = 13;
int myCounter3 = 0;

const int repeats = 3;
//SOUND aka TONE
const int buzzer1 = 6;
int buzzerCounter1 = 0;
int buzzerCounter2 = 0;

/*-----( Declare objects )-----*/
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass address of our oneWire instance to Dallas Temperature.
DallasTemperature sensors(&oneWire);
// Start the LCD display library
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
//custom chrachter for LCD
byte plusChar[8] = {
  0b00100,
  0b01110,
  0b00100,
  0b00000,
  0b11111,
  0b00000,
  0b01110,
  0b00000
};

byte degChar[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

/*-----( Declare Variables )-----*/
// Assign the addresses of your 1-Wire temp sensors.
// See the tutorial on how to obtain these addresses:
// http://www.hacktronics.com/Tutorials/arduino-1-wire-address-finder.html

DeviceAddress Probe01 = { 
  0x28, 0x18, 0x16, 0xD0, 0x05, 0x00, 0x00, 0x30 }; //HLT
DeviceAddress Probe02 = { 
  0x28, 0x0D, 0xF5, 0xE1, 0x05, 0x00, 0x00, 0xD3 }; //MASH
DeviceAddress Probe03 = { 
  0x28, 0x37, 0x37, 0xE5, 0x05, 0x00, 0x00, 0x5E }; //KETTLE
int strikeTempcalc() {
   int result;
   result = ((.2 / r) * (mashTemp - T1) + mashTemp)+ 4;
   return result;
 }
void setup()   /****** SETUP: RUNS ONCE ******/
{

  //-----Calculate Strike Temp----
  strikeTemp = strikeTempcalc();

  //------- Initialize the Temperature measurement library--------------
  sensors.begin();
  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster)
  sensors.setResolution(Probe01, 11);
  sensors.setResolution(Probe02, 11);
  sensors.setResolution(Probe03, 11);

  pinMode(bluePin1, OUTPUT);
  pinMode(greenPin1, OUTPUT);
  pinMode(redPin1, OUTPUT);
  pinMode(bluePin2, OUTPUT);
  pinMode(greenPin2, OUTPUT);
  pinMode(redPin2, OUTPUT);
  pinMode(redPin3, OUTPUT);
  pinMode(buzzer1, OUTPUT);
  digitalWrite(redPin3, LOW);
  digitalWrite(buzzer1, LOW);


  //---------------- Initialize the lcd ------------------
  // NOTE: Line number and character number start at 0 not 1
  lcd.begin (20, 4); // 20 characters, 4 lines
  lcd.createChar(1, plusChar);
  lcd.createChar(2, degChar);
  lcd.setCursor(0, 0);  //Start at character 0 on line 0 (aka top line)
  lcd.print(" BREW DAY! RDWHAHB! ");
  delay(1000);
  lcd.setCursor(2, 1);
  lcd.print("H = HLT/Strike");
  delay(500);
  analogWrite(bluePin1, 255);
  delay(250);
  analogWrite(bluePin1, 0);
  delay(250);
  analogWrite(greenPin1, 255);
  delay(250);
  analogWrite(greenPin1, 0);
  delay(250);
  analogWrite(redPin1, 255);
  delay(250);
  analogWrite(redPin1, 0);
  delay(250);
  delay(1000);
  lcd.setCursor(1, 2);
  lcd.print("T = Mash Tun/Temps");
  delay(500);
  analogWrite(bluePin2, 255);
  delay(250);
  analogWrite(bluePin2, 0);
  delay(250);
  analogWrite(greenPin2, 255);
  delay(250);
  analogWrite(greenPin2, 0);
  delay(250);
  analogWrite(redPin2, 255);
  delay(250);
  analogWrite(redPin2, 0);
  delay(250);
  delay(1000);
  lcd.setCursor(1, 3);
  lcd.print("K = Ktl/Boil Temps");
  delay(500);
  analogWrite(redPin3, 255);
  delay(250);
  analogWrite(redPin3, 0);
  delay(250);
  analogWrite(redPin3, 255);
  delay(250);
  analogWrite(redPin3, 0);
  delay(250);
  analogWrite(redPin3, 255);
  delay(250);
  analogWrite(redPin3, 0);
 
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("H calc : PREHEAT TUN");
  lcd.setCursor(0, 1);
  lcd.print("Grain Temp: ");
  lcd.print(T1);
  lcd.write((uint8_t)2);  
  lcd.setCursor(0, 2);
  lcd.print("wtr2grn: ");
  lcd.print(r);
  lcd.print("qts/lb");
  lcd.setCursor(0, 1);
  lcd.print("Mash Temp: ");
  lcd.print(mashTemp);
  lcd.write((uint8_t)2); 
  
  delay(3000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CURRENT  |");
  delay(100);
  lcd.print("ALARM| ");
  delay(100);
  lcd.print("SET");
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("H:");
  delay(100);
  lcd.setCursor(9, 1);
  lcd.print("|");
  delay(100);
  lcd.setCursor(15, 1);
  lcd.print("|");
  delay(100);
  lcd.print(strikeTemp);
  lcd.write((uint8_t)2);
  delay(100);
  lcd.setCursor(0, 2);
  lcd.print("T:");
  delay(100);
    lcd.setCursor(9, 2);
  lcd.print("|");
  delay(100);
  lcd.setCursor(15, 2);
  lcd.print("|");
  delay(100);
  lcd.print(mashTemp);
  lcd.write((uint8_t)2);
  delay(100);
  lcd.setCursor(0, 3);
  lcd.print("K:");
  delay(100);
    lcd.setCursor(9, 3);
  lcd.print("|");
  delay(100);
  lcd.setCursor(15, 3);
  lcd.print("|");
  delay(100);
  lcd.print(boilTemp);
  lcd.write((uint8_t)2);
  delay(1000);

Serial.begin(9600);

}
//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  float HLTtemp = (sensors.getTempF(Probe01));
  float MASHtemp = (sensors.getTempF(Probe02));
  float KETTLEtemp = (sensors.getTempF(Probe03));
  float mashTempmin = (mashTemp - 1.00);
  float mashTempmax = (mashTemp + 1.00);
  float strikeTempmin = (strikeTemp - 1.00);
  float strikeTempmax = (strikeTemp + 1.00);
  float strikeTempunder = (strikeTemp - 6.00);
  float boilTempmin = (boilTemp - 6);
  float boilTempmax = (boilTemp - 2);
Serial.println(strikeTemp);
 // Send the command to get temperatures
  sensors.requestTemperatures();
  delay(1000);

//Strike Temp
  lcd.setCursor(2, 1);
  lcd.print("       ");
  delay(100);
  lcd.setCursor(2, 1);
  displayTemperature(Probe01);
  lcd.setCursor(10, 1);
  lcd.print("*");
  delay(1000);
  
 /* Strike temp LED */
  if (HLTtemp < strikeTempunder) {
    analogWrite(bluePin1, 255);
    analogWrite(greenPin1, 0);
    analogWrite(redPin1, 0);
    lcd.setCursor(11, 1);
    lcd.print("COLD");
    delay(500);
    lcd.setCursor(10, 1);
    lcd.print(" ");

  }
//good
  else if (HLTtemp >= strikeTempunder && HLTtemp < strikeTempmin) { //strikeTempunder is Strike - 6 degrees strikeTempmin is Strike -1 degree
    analogWrite(bluePin1, 0);
    lcd.setCursor(11, 1);
    lcd.print("CLSE");
    if (myCounter1 < 3) {
          analogWrite(redPin1, 0);
          analogWrite(greenPin1, 0);
        for (int i = 0; i < repeats; i++) {
          analogWrite(redPin1, 255);
          analogWrite(greenPin1, 255);
          digitalWrite(buzzer1, HIGH);
          delay(300);
          digitalWrite(buzzer1, LOW);
          analogWrite(redPin1, 0);
          analogWrite(greenPin1, 0);
          delay(300);
        }
        myCounter1 = myCounter1 + 1;
      }
    analogWrite(greenPin1, 255);
    analogWrite(redPin1, 255);
    delay(100);
    lcd.setCursor(10, 1);
    lcd.print(" ");

  }
//DONE
  else if (HLTtemp > strikeTempmin && HLTtemp < strikeTempmax) {
      analogWrite(bluePin1, 0);
      analogWrite(greenPin1, 255);
      analogWrite(redPin1, 0);
      lcd.setCursor(11, 1);
      lcd.print("+/-1");
      delay(500);
      lcd.setCursor(10, 1);
      lcd.print(" ");

    }
//DONE
  else if (HLTtemp > strikeTempmax) {
      analogWrite(bluePin1, 0);
      analogWrite(greenPin1, 0);
      analogWrite(redPin1, 255);
      lcd.setCursor(11, 1);
      lcd.print("2HOT");
      delay(500);
      lcd.setCursor(10, 1);
      lcd.print(" ");

    }
  else {
      lcd.setCursor(11, 1);
      lcd.print("ERR ");
      delay(100);
      lcd.setCursor(10, 1);
      lcd.print(" ");

    }
//DONE      

//Mash temp  
  delay(500);
  lcd.setCursor(2, 2);
  lcd.print("       ");
  delay(100);
  lcd.setCursor(2, 2);
  displayTemperature(Probe02);
  lcd.setCursor(10, 2);
  lcd.print("*");
  delay(1000);
   /* MASH temp LED */

     if (MASHtemp < mashTempmin) {
      analogWrite(bluePin2, 255);
      analogWrite(greenPin2, 0);
      analogWrite(redPin2, 0);
      lcd.setCursor(11, 2);
      lcd.print("COLD");
      delay(500);
      lcd.setCursor(10, 2);
      lcd.print(" ");

    }
//DONE
    else if (MASHtemp > mashTempmin && MASHtemp < mashTempmax) {
      analogWrite(bluePin2, 0);
      analogWrite(greenPin2, 255);
      analogWrite(redPin2, 0);
      lcd.setCursor(11, 2);
      lcd.print("+/-1");
      delay(500);
      lcd.setCursor(10, 2);
      lcd.print(" ");
    }
//DONE
    else if (MASHtemp > mashTempmax) {
      analogWrite(bluePin2, 0);
      analogWrite(greenPin2, 0);
      lcd.setCursor(11, 2);
      lcd.print("2HOT");

      if (myCounter2 < 3) {
        analogWrite(redPin2, 0);
        for (int i = 0; i < repeats; i++) {
          analogWrite(redPin2, 255);
          delay(150);
          analogWrite(redPin2, 0);
          delay(150);
        }
        myCounter2 = myCounter2 + 1;
      }
      analogWrite(redPin2, 255);
      delay(500);
      lcd.setCursor(10, 2);
      lcd.print(" ");
    }
//DONE
    else {
      lcd.setCursor(11, 2);
      lcd.print("ERR ");
      delay(100);
      lcd.setCursor(10, 2);
      lcd.print(" ");
    }
//DONE 

//Boil Temp 
  delay(500);
  lcd.setCursor(2, 3);
  lcd.print("       ");
  delay(100);
  lcd.setCursor(2, 3);
  displayTemperature(Probe03);
  lcd.setCursor(10, 3);
  lcd.print("*");
  delay(1000);
   /* BOIL temp LED */
 
    if (KETTLEtemp < boilTempmin) {
      digitalWrite(redPin3, LOW);
      lcd.setCursor(11, 3);
      lcd.print("COLD");
      delay(500);
      lcd.setCursor(10, 3);
      lcd.print(" ");
    }
//DONE
    else if (KETTLEtemp > boilTempmin && KETTLEtemp < boilTempmax) {
      lcd.setCursor(11, 3);
      lcd.print("CLSE");
      if (myCounter3 < 3) {
          digitalWrite(redPin3, LOW);
        for (int i = 0; i < repeats; i++) {
          digitalWrite(redPin3, HIGH);
          digitalWrite(buzzer1, HIGH);
          delay(300);
          digitalWrite(buzzer1, LOW);
          digitalWrite(redPin3, LOW);
          delay(300);
        }
        myCounter3 = myCounter3 + 1;
      }
      digitalWrite(redPin3, HIGH);
      delay(500);
      lcd.setCursor(10, 3);
      lcd.print(" ");


    }

    else if (KETTLEtemp > boilTempmax) {
      digitalWrite(redPin3, HIGH);
      lcd.setCursor(11, 3);
      lcd.print("BOIL");
      delay(500);
      lcd.setCursor(10, 3);
      lcd.print(" ");

    }
    else {
     lcd.setCursor(11, 3);
      lcd.print("ERR ");
      delay(100);
      lcd.setCursor(10, 3);
      lcd.print(" ");
    }
 
}
//--(end main loop )---


/*-----( Declare User-written Functions )-----*/
void displayTemperature(DeviceAddress deviceAddress){

  float tempF = sensors.getTempF(deviceAddress);

  if (tempF == -196.00) // Measurement failed or no device found
  {
    lcd.print("Err");
  }
  else
  {
    lcd.print(tempF);
    lcd.write((uint8_t)2);
  }
}// End printTemperature







//*********( THE END )***********
