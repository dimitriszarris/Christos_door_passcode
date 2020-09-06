#include <Keypad.h>
#include <avr/sleep.h>//this AVR library contains the methods that controls the sleep modes

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {3, 4, 5, 6};
byte colPins[COLS] = {7, 8, 9};
byte greenPin = 10;
byte redPin = 12;
byte buzzerPin = 11;
const byte wakeUpPin = 2;

const char* cPassword = "123445566778899";
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
void setup() {
  Serial.begin(9600);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(wakeUpPin, INPUT);
  digitalWrite(LED_BUILTIN,HIGH);//turning LED on
  goingToSleep();
}

void loop() {
  wakeUpKeypad();
  goingToSleep();
}

void goingToSleep() {
    sleep_enable();//Enabling sleep mode
    attachInterrupt(0, wakeUp, HIGH);//attaching a interrupt to pin d2
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
    digitalWrite(LED_BUILTIN,LOW);//turning LED off
    delay(1000); //wait a second to allow the led to be turned off before going to sleep
    sleep_cpu();//activating sleep mode
    Serial.println("just woke up!");//next line of code executed after the interrupt 
    Serial.println("Interrrupt Fired");//Print message to serial monitor
    sleep_mode();
    sleep_disable();//Disable sleep mode
    detachInterrupt(0); //Removes the interrupt from pin 2;
    digitalWrite(LED_BUILTIN,HIGH);//turning LED on
}

void wakeUp() {
}

void wakeUpKeypad() {
  char* password;

  noTone(buzzerPin);
  
  char key = getValidKey();

  if(key == '*') {
    Serial.println("Start recording...");
    password = getPassword();
    if(verifyPassword(password)) {
      digitalWrite(greenPin, HIGH);
      Serial.print("Password passed: ");
      Serial.println(password);
      buzzCorrectPassword();
      digitalWrite(greenPin, LOW);
    } else {
      digitalWrite(redPin, HIGH);
      Serial.print("Wrong password: ");
      Serial.println(password);
      buzzWrongPassword();
      digitalWrite(redPin, LOW);
    }
  }
}

char getValidKey() {
  char key;
  while(true) {
    key = keypad.getKey();
    if(key)
      return key;
  }

  return 0;
}

char* getPassword() {
  char* password = new char(33);
  memset(password, 0, 33);
  int i = 0;
  char key;
  key = getValidKey();
  while(key != '*' && i <= 32) {
    password[i] = key;
    i++;
    key = getValidKey();
  }

  Serial.println("Finished recording...");
  return password;
}

void buzzCorrectPassword() {
  tone(buzzerPin, 500); //turn the buzzer on
  delay(200); //wait for 5 milliseconds 
  tone(buzzerPin, 300); //turn the buzzer on
  delay(500); //wait for 5 milliseconds 
  noTone(buzzerPin);
  delay(3000);
}

void buzzWrongPassword() {
  for(int t = 0; t < 3; t++) {
    //frequence loop from 200 to 800
    for(int i = 200;i <= 300;i++) {
      tone(buzzerPin,i); //turn the buzzer on
      delay(1); //wait for 5 milliseconds 
    }
    delay(50); //wait for 4 seconds on highest frequence
    //frequence loop from 800 downto 200
    for(int i = 300;i >= 200;i--) {
      tone(buzzerPin,i);
      delay(3);
    }
  }
  noTone(buzzerPin);
  delay(3000);
}

bool verifyPassword(char* password) {
  if(strncmp(password, cPassword, strlen(cPassword)) == 0)
    return true;

  return false;
}
