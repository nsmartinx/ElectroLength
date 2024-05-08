#include<cmath>

const double resistance = 6100000.0;
const double voltageCutoff = 0.864665;
const double voltageSource = 5.7;
const double maxRead = 867.0;
const double minCapacitance = 255;
const double maxCapacitance = 330;
const int num = 100;

double capacitanceMeasurements[num] = {0};
int currentMeasurementIndex = 0;


void setup() {
  pinMode(13, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int startTime;
  double voltage;

  digitalWrite(13, HIGH);
  
  startTime = micros();
  double time;
  while (true) {
    voltage = analogRead(A0) * voltageSource / maxRead;

    time = micros() - startTime;
    if (voltage > voltageCutoff * voltageSource) {
      break;
    }
    if (time > 5000000) {
      Serial.println("Error: too long to charge");
      break;
    }
  }

  double measuredCapacitance = time / (2 * resistance) * 1000000;
  capacitanceMeasurements[currentMeasurementIndex % num] = measuredCapacitance;
  currentMeasurementIndex++;

  

  if(currentMeasurementIndex % num == 0){
    double averageCapacitance = calculateAverage();
    Serial.print("Measured Capacitance: ");
    Serial.print(averageCapacitance);
    Serial.println(" pico Farads");
    Serial.print("Estimated Insertion: ");
    Serial.print((averageCapacitance - minCapacitance) / (maxCapacitance - minCapacitance) * 100);
    Serial.println("%");
  }

  digitalWrite(13, LOW);
  
  startTime = micros();
  while (true) {
    voltage = analogRead(A0) * 5.0 / 1023.0;
    if (voltage < 0.01) {
      break;
    }
    if (micros() - startTime > 5000000) {
      Serial.println("Error: too long to discharge");
      break;
    }
  }
}

double calculateAverage() {
  double sum = 0;
  int less = 0;
  for (int i = 0; i < num; i++) {
    if(capacitanceMeasurements[i] < 450 && capacitanceMeasurements[i] > 200){
      sum += capacitanceMeasurements[i];
    }
    else less++;
  }
  if(less == num){
    return -1;
  }
  return sum / (num - less);
}
