#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <DHT.h>
#include <Ds1302.h>

//SD Card 
const int chipSelect = 10;

// Define Sensors
#define NUM_SENSORS 5
int dhtPins[NUM_SENSORS] = {2, 3, 4, 5, 6};
DHT dht[NUM_SENSORS] = { 
   DHT(2, DHT22),DHT(3, DHT22), DHT(4, DHT22), DHT(5, DHT22), DHT(6, DHT22)
};

//Create RTC Instance
#define DS1302_CLK 9
#define DS1302_DAT 7
#define DS1302_RST 8
Ds1302 rtc(DS1302_RST, DS1302_CLK, DS1302_DAT);


//known temperatures only used for calculateOffsed function:
const int numPoints = 3;
float tempPoints[numPoints] = {7.9, 19.1, 22.1};  
float offsetPoints[numPoints] = {1.9, 2.7, 1.1};
//regresssion coefficients -> calculate .py script
const float a = -0.00219;
const float b = 0.00935;
const float c = 1.96278;

//define RTC Weekdays
const static char* WeekDays[] =
{
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday"
};

void setup() {
  Serial.begin(9600);
  Serial.println("DHT22 Multi-Sensor Offset Calibration");
  
  //RTC
  rtc.init();
  // Temp Sensors
  for (int i = 0; i < NUM_SENSORS; i++) {
    dht[i].begin();
  }
  //SD Card
  if (!SD.begin(chipSelect)) { //skips remaining void setup if not successful 
    Serial.println("SD-Karte konnte nicht initialisiert werden!");
    return;
  }
  Serial.println("SD-Karte erfolgreich initialisiert.");
}

void loop() {

  float temperatures[NUM_SENSORS];
  int validCount = 0;

  // Read temperature 
  for (int i = 0; i < NUM_SENSORS; i++) {
    float temp = dht[i].readTemperature();
    if (!isnan(temp)) {
      //Serial.print(i+1);
      //Serial.print(": ");
      //Serial.println(temp);
      temperatures[validCount++] = temp;

    }
    else {
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.println(" failed!");
    }
  }

  if (validCount > 0) {
    float rawTemperature = calculateMean(temperatures, validCount);
    float offset = calculateOffset(rawTemperature);
    float correctedTemperature = rawTemperature - offset;

    saveOnSD(rawTemperature, offset, correctedTemperature);
    displayResult(rawTemperature, offset, correctedTemperature);
  }
  else {
    Serial.println("All sensors failed to read!");
  }
  delay(60000);
}

void saveOnSD(float rawTemperature, float offset, float correctedTemperature) {
  File dataFile = SD.open("datenlog.txt", FILE_WRITE);
  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  static uint8_t last_second = 0;

  if (dataFile) {
    dataFile.print("20");
    dataFile.print(now.year);
    dataFile.print('-');
    if (now.month < 10) dataFile.print('0');
    dataFile.print(now.month);
    dataFile.print('-');
    if (now.day < 10) dataFile.print('0');
    dataFile.print(now.day);
    dataFile.print(' ');
    dataFile.print(WeekDays[now.dow - 1]);
    dataFile.print(' ');
    if (now.hour < 10) dataFile.print('0');
    dataFile.print(now.hour);
    dataFile.print(':');
    if (now.minute < 10) dataFile.print('0');
    dataFile.print(now.minute);
    dataFile.print(':');
    if (now.second < 10) dataFile.print('0');
    dataFile.print(now.second);
    dataFile.println();
    dataFile.println();

    dataFile.print("Raw Temperature: ");
    dataFile.print(rawTemperature);
    dataFile.println(" °C");

    dataFile.print("Applied Offset: ");
    dataFile.print(offset);
    dataFile.println(" °C");

    dataFile.print("Final Corrected Temperature: ");
    dataFile.print(correctedTemperature);
    dataFile.println(" °C");
    dataFile.println("----------------------------------------");
    dataFile.close();
    Serial.println("Daten wurden gespeichert.");
  } else {
    Serial.println("Fehler beim Öffnen der Datei!");
  }
}

void displayResult(float rawTemperature, float offset, float correctedTemperature) {
  Ds1302::DateTime now;
  rtc.getDateTime(&now);
  static uint8_t last_second = 0;

  Serial.print("20");
  Serial.print(now.year);
  Serial.print('-');
  if (now.month < 10) Serial.print('0');
  Serial.print(now.month);
  Serial.print('-');
  if (now.day < 10) Serial.print('0');
  Serial.print(now.day);
  Serial.print(' ');
  Serial.print(WeekDays[now.dow - 1]);
  Serial.print(' ');
  if (now.hour < 10) Serial.print('0');
  Serial.print(now.hour);
  Serial.print(':');
  if (now.minute < 10) Serial.print('0');
  Serial.print(now.minute);
  Serial.print(':');
  if (now.second < 10) Serial.print('0');
  Serial.print(now.second);
  Serial.println();
  Serial.println();

  Serial.print("Raw Temperature: ");
  Serial.print(rawTemperature);
  Serial.println(" °C");

  Serial.print("Applied Offset: ");
  Serial.print(offset);
  Serial.println(" °C");

  Serial.print("Final Corrected Temperature: ");
  Serial.print(correctedTemperature);
  Serial.println(" °C");
  Serial.println("----------------------------------------");
}

float calculateMean(float arr[], int size) {
  float sum = 0.0;
  for (int i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum / size;
}

// **Linear Interpolation Function** -> nicht genau
float calculateOffset(float temp) {
  for (int i = 0; i < numPoints - 1; i++) {
    if (temp >= tempPoints[i] && temp <= tempPoints[i + 1]) {
      float temp1 = tempPoints[i];
      float temp2 = tempPoints[i + 1];
      float offset1 = offsetPoints[i];
      float offset2 = offsetPoints[i + 1];

      // Linear interpolation formula
      return offset1 + ((offset2 - offset1) / (temp2 - temp1)) * (temp - temp1);
    }
  }

  if (temp < tempPoints[0]) return offsetPoints[0];
  if (temp > tempPoints[numPoints - 1]) return offsetPoints[numPoints - 1];

  return 0.0;
}

//Quadratic regresssion coefficients
float correctTemperature(float measuredTemp) {
    float offset = a * measuredTemp * measuredTemp + b * measuredTemp + c; //O = aT^2 + bT + c
    float correctedTemp = measuredTemp - offset;
    return correctedTemp;
}