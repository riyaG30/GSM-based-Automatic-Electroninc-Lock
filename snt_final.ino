#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h> 
SoftwareSerial SIM800L(2, 3);
Servo ServoMotor;
int position = 0;
int incorrectPasswordCount = 0;
int incorrectNumberCount = 0;
LiquidCrystal lcd(14, 15, 16, 17, 18, 19);
char* password = "888";
int RedpinLock = 12;
int GreenpinUnlock = 13;
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6,7,8};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
void setup() {
  SIM800L.begin(9600);
  // put your setup code here, to run once:
  ServoMotor.attach(11);
  LockedPosition(true);
  lcd.begin(16, 2);

  // print the password on the LCD
  lcd.print("Password:");
  lcd.setCursor(0, 0);
  lcd.print(password);

}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,0);
  lcd.print("Enter the password: ");
  lcd.setCursor(0,1);
  while (SIM800L.available()) {
    Serial.write(SIM800L.read()); 
  }
  if (SIM800L.available() > 0) {
    String incomingMessage = SIM800L.readString();
    if (incomingMessage.indexOf("AT+CMGL") >= 0) {
      SIM800L.println("AT+CMGD=1,4");
    }
  }
  //lcd.print(password);
  char key = keypad.getKey();
  if (key == '*' || key == '#'){
    position = 0;
    LockedPosition(true);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter the password:");
    lcd.setCursor(0,1):
  }
  if (key == password[position]){
    lcd.setCursor(position,1);
    lcd.print(key);
    position ++;
  }else{
    incorrectNumberCount ++;
    lcd.setCursor(postition,1);
    lcd.print(key);
    position ++;
  }
  if (position == 3 && incorrectNumberCount == 0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Correct Password");
    delay(100);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter the password:");
    lcd.setCursor(0,1);
    LockedPosition(false);
  }else{
    incorrectPasswordCount ++;
    LockedPosition(true);
  }
  if (incorrectPasswordCount == 3){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Incorrect Password");
    delay(100);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter the password:");
    lcd.setCursor(0,1);
    incorrectPasswordCount = 0;
    SIM800L.println("AT+CMGF=1");
    SIM800L.println("AT+CMGS=\"+918709921638\"");
    SIM800L.println("Unauthorized access attempted!");
    SIM800L.println((char)26); 
    delay(5000);  
    SIM800L.println("AT+CMGD=1,4");
    while (1) {}
  }
  delay(100);
}

  void LockedPosition(int locked){
    if (locked){
      digitalWrite(RedpinLock, HIGH);
      digitalWrite(GreenpinUnlock, LOW);
      ServoMotor.write(11);
    }else{
      digitalWrite(RedpinLock, LOW);
      digitalWrite(GreenpinUnlock, HIGH);
      ServoMotor.write(90);
    }
 }

