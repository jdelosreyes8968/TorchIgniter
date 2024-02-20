int count = 0;
char inputWords[50]; //Will accept 50 characters before attempting to run again

const int sensorPin1 = A0;
const int sensorPin2 = A1;
const int sensorPin3 = A2;

const int oxSolenoidPin = 7;
const int fuelSolenoidPin = 8;
const int glowPlug = 9;

int delayVal;

float pressure1 = 0;
float pressure2 = 0;
float pressure3 = 0;

struct Pair {
  char name[10]; //max name length is 10
  float value;
};

Pair pairs[4];

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(oxSolenoidPin, OUTPUT);
  pinMode(fuelSolenoidPin, OUTPUT);
  pinMode(glowPlug, OUTPUT);
  //Creating names for sorting
  resetNames();
  //Set default state
  turnOff(oxSolenoidPin);
  turnOff(fuelSolenoidPin);
  turnOff(glowPlug);
}

void loop() {
  switch(count) {
    case 0:
      takeInputValues();
      break;

    case 1:
      takeInputValues();
      break;

    case 2:
      takeInputValues();
      break;

    case 3:
      takeInputValues();
      break;

    case 4:
      bubbleSort(); //sort values
      break;

    case 5:
      //clearing input words
      for(int i = 0; i < sizeof(inputWords); i++) {
        inputWords[i] = 0;
      }
      printInfo();
      break;

    case 6:
      for(int i = 0; i < 4; i++) {
        takePressure(i);
        findAction(i);
      }
      count++;
      break;
      
    case 7:
      return;
      break;
  }
  delay(100);
}

  void bubbleSort() { //bubble sort is 0(N^2) but there are only 4 values so it's fine
    int n = 4; //4 entry values

    for(int i = 0; i < n - 1; i++) {
      for(int j = 0; j < n - 1; j++) {
        if(pairs[j].value > pairs[j + 1].value) {
          Pair temp = pairs[j];
          pairs[j] = pairs[j + 1];
          pairs[j + 1] = temp;
        }
      }
    }
    if(strcmp(pairs[0].name, "Glow Plug") != 0 || strcmp(pairs[3].name, "Shut Off") != 0) {
      Serial.println("Glow Plug must be first action and Shut Off must be final action, please reenter values");
      resetNames();
      count = 0;
    }
    else {
      count++; //Only move on if order is acceptable
    }
  }
  void resetNames() {
    strcpy(pairs[0].name, "Glow Plug");
    strcpy(pairs[1].name, "Ox");
    strcpy(pairs[2].name, "Fuel");
    strcpy(pairs[3].name, "Shut Off");
  }
  void takeInputValues() {
      Serial.println("Enter " + String(pairs[count].name) + " Time:");
      while (Serial.available() == 0) {
        // Do nothing
      }
      pairs[count].value = Serial.parseFloat();
      
      if (Serial.read() == '\n') {
        if (pairs[count].value <= 0) {
          Serial.println("Error: Negative/Zero Input Value");
          pairs[count].value = 0; //reset value
        }
        else {
          // Display the parsed value
          Serial.println(String(pairs[count].name) + " set to " + String(pairs[count].value) + " seconds");
          count++;
        }
      }
      else {
        // If the input does not end with a newline character, print an error message
        Serial.println("Error: Invalid input format");
      }
  }
  void printInfo() {
    //Print statements
    Serial.println("--------------------");
    Serial.println("Info Entered");
    for(int i = 0; i < 4; i++) { //output names and times in order
      Serial.println(String(pairs[i].name) + " Time: " + String(pairs[i].value));
    }
    Serial.println("Enter \"FIRE\" to fire");
    Serial.println("Enter \"RESTART\" to restart");

    while (Serial.available() == 0) {
      // Do nothing
    }
    
    Serial.readBytesUntil('\n', inputWords, sizeof(inputWords) - 1);
    if (strcmp(inputWords, "FIRE") == 0) {
      Serial.println("--------------------");
      Serial.println("Firing");
      count++;
    }
    else if(strcmp(inputWords, "RESTART") == 0) {
      Serial.println("--------------------");
      Serial.println("Restarting");
      Serial.println("--------------------");
      count = 0; //resetting count
      resetNames();
    }
    else {
      Serial.println("--------------------");
      Serial.println("Invalid input, please try again");
    }
  }
  float getNextDelay(int pos) {
    if(pos == 0) {
      float time = pairs[pos].value * 1000;
      return time;
    }
    else {
      float time = (pairs[pos].value - pairs[pos - 1].value) * 1000;
      return time;
    }
  }
  void takePressure(int i) {
    delayVal = getNextDelay(i);
    while(delayVal > 0) {
      
      //Take input from sensors
      int sensorValue1 = analogRead(sensorPin1);
      int sensorValue2 = analogRead(sensorPin2);
      int sensorValue3 = analogRead(sensorPin3);
      
      //Take input from sensorValue and scale it (from 0->1023 to 1->500)
      pressure1 = (500.0 / 1023.0) * float(sensorValue1);
      pressure2 = (500.0 / 1023.0) * float(sensorValue2);
      pressure3 = (500.0 / 1023.0) * float(sensorValue3);
     
      //Print pressures
      Serial.print("Pressure1: ");
      Serial.print(pressure1);
      Serial.print(" | Pressure2: ");
      Serial.print(pressure2);
      Serial.print(" | Pressure3: ");
      Serial.println(pressure3);
      
      delay(10);
      delayVal -= 10;
    }
  }
  void findAction(int pos) {
    if(strcmp(pairs[pos].name, "Ox") == 0) {
      doOx();
    }
    else if(strcmp(pairs[pos].name, "Glow Plug") == 0) {
      doGlowPlug();
    }
    else if(strcmp(pairs[pos].name, "Fuel") == 0) {
      doFuelOn();
    }
    else if(strcmp(pairs[pos].name, "Shut Off") == 0) {
      doShutOff();
    }
  }
  void doOx() {
    Serial.println("Oxxing");
    turnOn(oxSolenoidPin);
  }
  void doGlowPlug() {
    Serial.println("Glow Plugging");
    turnOn(glowPlug);
  }
  void doFuelOn() {
    Serial.println("Fueling");
    turnOn(fuelSolenoidPin);
  }
  void doShutOff() {
    Serial.println("Shutting Off");
    turnOff(oxSolenoidPin);
    turnOff(fuelSolenoidPin);
    turnOff(glowPlug);
  }
  
  void turnOn(int name) {
    digitalWrite(name, LOW);
  }
  void turnOff(int name) {
    digitalWrite(name, HIGH);
  }
