// Plant water project

// include the library code:
#include <LiquidCrystal.h>
// include for moisture sensor
#include <dht11.h>
dht11 DHT;
#define DHT11_PIN 11

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(10, 9, 5, 4, 7, 2);

// set up a constant for the tilt switch pin
const int switchPin = 8;

// variable to hold the value of the switch pin
int switchState = 0;

// variable to hold previous value of the switch pin
int prevSwitchState = 0;

// a variable to choose which reply from the crystal ball
int reply;

int hits = 0;

// variable to hold sensor value
int sensorValue;
// variable to calibrate low value
int sensorLow = 1023;
// variable to calibrate high value
int sensorHigh = 0;

// light sensor value
int lightVal;

// moisture sensor value
int tempHumidityVal;

// moisture sensor value
int moistureVal;

// LED pin
const int ledRedPin = 13;
const int ledBluePin = 3;
const int ledGreenPin = 6;
const int ledYellowPin = 12;
const int pumpDigitalPin = 17; // Analog pin A3

void setup() {
  // set up the number of columns and rows on the LCD
  lcd.begin(16, 2);
  Serial.begin(9600);

  // set up the switch pin as an input
  pinMode(switchPin, INPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledYellowPin, OUTPUT);
  pinMode(pumpDigitalPin, OUTPUT);
}

void printSensorStartValues() {
  Serial.println(lightVal,DEC);// print initial value of light sensor
  Serial.println(tempHumidityVal,DEC); // print initial value of moisture sensor
  Serial.println(DHT11LIB_VERSION);
  Serial.println();
  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

void loop() {
  // this keeps checking the moisture to activate/deactivate the pump
  digitalWrite(pumpDigitalPin, HIGH);
  checkMoistureForPump();
  // this manages the click of the switch button 
  clickSwitchButton();  
}

void clickSwitchButton(){
  // check the status of the switch
  switchState = digitalRead(switchPin);

  if(switchState != prevSwitchState) {
    if (switchState == LOW) {
          hits = hits + 1;
          delay(10);
    }
  }

  switch(hits){
    case 0:
   //Serial.println("hits = 0");
    readTemperature();
    break;

    case 1:
    readHumidity();
    break;
    
    case 2:
    infoMoisture();
    break;

    case 3:
    readLight();
    break;    

    default:
    hits = 0;
    break;
  }
  
  prevSwitchState = switchState;
  delay(100);
}

void readLight() {
  lightVal = analogRead(A0);

  // DISPLAY DATA ON LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Light: "); lcd.print(lightVal, DEC); lcd.print(" Lux");
  lcd.setCursor(0,1);
  if(lightVal < 700) {
    digitalWrite(ledGreenPin, HIGH);
    lcd.print("need light");
  }
  else {
    digitalWrite(ledGreenPin, HIGH);
    lcd.print("sunburn!");
  }
}

void readTemperature() {
  int chk;
  Serial.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);    // READ DATA
  switch (chk){
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
 // DISPLAT DATA ON SERIAL
  int temperature = DHT.temperature;
  Serial.println(temperature,1);

  // DISPLAY DATA ON LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: "); lcd.print(temperature, 1); lcd.print("C");
  lcd.setCursor(0, 1);
  if(temperature < 10) {
    digitalWrite(ledBluePin, HIGH);
    lcd.print("I'm cold!");
  }
  else if(temperature >= 11 && temperature < 40) {
    digitalWrite(ledBluePin, LOW);
    lcd.print("Nice weather :)");
  }
  else {
    digitalWrite(ledBluePin, HIGH);
    lcd.print("It's too hot!");
  }
}

void readHumidity() {
  int chk;
  Serial.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);    // READ DATA
  switch (chk){
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
 // DISPLAT DATA ON SERIAL
  int humidity = DHT.humidity;
  Serial.print(humidity,1);
  // DISPLAY DATA ON LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Humid: "); lcd.print(humidity, 1); lcd.print("%");
  lcd.setCursor(0, 1);
  if(humidity < 30) {
    digitalWrite(ledYellowPin, HIGH);
    lcd.print("dry weather..");
  }
  else if (humidity >= 11 && humidity < 50) {
    digitalWrite(ledYellowPin, LOW);
    lcd.print("nice weather");
  }
  else {
    digitalWrite(ledYellowPin, HIGH);
    lcd.print("Too humid :O");
  }
}

void resetAllLed() {
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  digitalWrite(ledYellowPin, LOW);
  digitalWrite(ledBluePin, LOW);
}

void infoMoisture() {
  moistureVal = analogRead(A2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Moisture: "); lcd.print(moistureVal, DEC);
  lcd.setCursor(0,1);
  if(moistureVal >=0 && moistureVal <= 300) {
    lcd.print("too dry!");
    digitalWrite(ledRedPin, HIGH);
    digitalWrite(pumpDigitalPin, HIGH); // activate pump
  }
  else if(moistureVal >300 && moistureVal <=700) {
    lcd.print("humid");
    digitalWrite(ledRedPin, LOW);
    digitalWrite(pumpDigitalPin, LOW); // deactivate pump 
  }
  else {
    digitalWrite(ledRedPin, HIGH);
    lcd.print("I'm drowning :(");
  }
}

void checkMoistureForPump() {
  moistureVal = analogRead(A2);
  if(moistureVal >=0 && moistureVal <= 300) {
    digitalWrite(pumpDigitalPin, HIGH); // activate pump
  }
  else if(moistureVal >300 && moistureVal <=700) {
    digitalWrite(pumpDigitalPin, LOW); // deactivate pump 
  }
}
