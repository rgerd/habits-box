int sensorPin = A0;

// TODO: Rolling average circular buffer

void setup() {
 Serial.begin(115200); 
 analogReadResolution(12);
 pinMode(4, OUTPUT);
 digitalWrite(4, LOW);
}

float currentAverage;
int chosenPressIndex;
int currentPressIndex;
int currentPressIndexCount;

void loop() {
  float currentReading = (float)analogRead(sensorPin);
  currentAverage = (currentReading * 0.005) + (currentAverage * 0.995);
  // Serial.println(currentAverage);
  int pressIndex = getPressIndex((int)(currentAverage));

  if (pressIndex == currentPressIndex) {
    if (currentPressIndexCount < 100) {
      currentPressIndexCount++; 

      if (currentPressIndexCount == 100) {
        chosenPressIndex = currentPressIndex;
        Serial.print("Current press: ");
        Serial.println(chosenPressIndex, BIN);
      }
    }
  } else {
    if(pressIndex >= 0) {
      currentPressIndex = pressIndex;
      currentPressIndexCount = 0; 
    }
  }

  outputLight(currentPressIndexCount == 100 ? HIGH : LOW);
    
  delay(2);
}


int lightOutput = LOW;
void outputLight(int outputValue) {
  if (lightOutput != outputValue) {
    lightOutput = outputValue;
    digitalWrite(4, lightOutput);
  }
}

int numInputs = 4;
int numInputCombos = 1 << numInputs;

//# Orange = 0
//# Red = 1
//# Blue = 2
//# Purple = 3

int inputRanges[] = {
/*0000*/ 0, 30,
/*0001*/ 530, 550, /* 22k, ReReOr */
/*0010*/ 1010, 1030, /* 10k, BrBlOr */
/*0011*/ 1330, 1350,
/*0100*/ 1680, 1700, /* 4.7k, YeViRe */
/*0101*/ 1880, 1900,
/*0110*/ 2070, 2090,
/*0111*/ 2220, 2240,
/*1000*/ 260, 280, /* 47k, YeViOr */
/*1001*/ 730, 750,
/*1010*/ 1170, 1190,
/*1011*/ 1450, 1470,
/*1100*/ 1780, 1800,
/*1101*/ 1960, 1980,
/*1110*/ 2150, 2170,
/*1111*/ 2280, 2300
};

int getPressIndex(int reading) {
    for (int i = 0; i < numInputCombos; i++) {
      if (reading > inputRanges[i * 2 + 0] && reading < inputRanges[i * 2 + 1]) {
        return i;
      }
    }
    return -1;
}
