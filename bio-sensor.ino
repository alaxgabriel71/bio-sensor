#include <dht.h>
#include <LiquidCrystal.h>

#define dht_pin 3 // defines digital pin 3 as DHT sensor pin
#define temperature_limit 50 // temperature limit to start the alert message

String comdata = "";

dht dht11; // create a object from DHT sensors library

int led_alert = 13; // the alert LED defined at pin 13

int temperature = 0x00, // stores temperature value
      humidity  = 0x00; // stores humidity value

//LiquidCrystal lcd(RS, E, D4, D5, D6, D7); control pinout of JHD162A LCD module
LiquidCrystal lcd(12, 11, 4, 5, 6, 7);

// prototype of the function of printing parameters on the LCD
void printToLcd(int temp, int humid);

void setup() {
  pinMode(led_alert, OUTPUT); // LED pin defined as output

  Serial.begin(9600); // starts serail communication with 9600 of baud rate 

  lcd.begin(16,2); // starts the LCD as 16 columns and 2 rows
}

void loop() {
  dht11.read11(dht_pin); // defines the model of the DHT sensor as 11 and starts the read
  temperature = dht11.temperature; // receives the temperature value
  humidity = dht11.humidity; // receives the humidity value
  
  // print to serial monitor
  Serial.print("Temperatura:    ");
  Serial.print(temperature);
  Serial.println(" °C");

  // turns on the alert if the temperature is above the limit
  if(temperature> temperature_limit){
    digitalWrite(led_alert, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("    ALERTA!!    ");
    lcd.setCursor(0,1);
    lcd.print("TEMP.: ");
    lcd.print(temperature);
    lcd.print(" ");
    lcd.print((char)223);
    lcd.print("C");
    delay(3000);
    digitalWrite(led_alert, LOW);
  }
  printToLcd(temperature, humidity); 
}

// a function to print temperature and humidity on the LCD screen
void printToLcd(int temp, int humid) {
  lcd.setCursor(0,0);
  lcd.print("Temp.: ");
  lcd.print(temp);
  lcd.print(" ");
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Um. R.: ");
  lcd.print(humid);
  lcd.print(" %");
  delay(2000);

}

void serialEvent(){
  comdata="";
  while(Serial.available())
  {
    comdata += char(Serial.read());
    delay(2);
  }
}