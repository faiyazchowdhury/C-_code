// constants for pin numbers
const int buttonPin = PUSH2;     // the number of the pushbutton pin
const int ledPin =  GREEN_LED;      // the number of the LED pin

// variables will change:
int walletWasClosed = 0; // for wallet toggling.
int x = 0; // to count time.

void setup() {
  pinMode(ledPin, OUTPUT);      
  pinMode(buttonPin, INPUT_PULLUP);     
}

// the loop function runs over and over again forever
void loop() {      
  walletClosed();
}

void walletClosed() {
   x = 0;
  while (digitalRead(buttonPin) == HIGH){
    if (x == 1000) {
      alarm();
    }
    delay(1000);   // wait for a second
    x++;
    
  }
  walletOpen();
}

void walletOpen() {
  int x = 0;
  while (digitalRead(buttonPin) == LOW) {
    if (x == 200) {
      alarm();
    }
    x++;
    delay(1000);   // wait for a second
  }
  walletClosed();
}

void alarm() {
  boolean walletWasClosed = digitalRead(buttonPin);
  while (walletWasClosed == digitalRead(buttonPin)) {
    digitalWrite(ledPin, HIGH);  
  }
  if (digitalRead(buttonPin) == HIGH) {
    walletClosed();
  }
  else {
    walletOpen();
  }
}
