#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1016) // Adjust this value based on weather forcast or run it before

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

float prevAltitude = 0;
unsigned long prevTime = 0;

// EMA filter variables
float filteredSpeed = 0;
float alpha = 0.3; // smoothing factor (0 = very smooth, 1 = no filter)

void setup() {
    Serial.begin(9600);
    while (!Serial);    // time to get serial running
    Serial.println(F("BME280 test"));

    unsigned status;
    status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }

    Serial.println("-- Default Test --");
    delayTime = 200; // Change this value based on how frequent we want to get the values

    // Initialize previous altitude & time
    prevAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    prevTime = millis();

    Serial.println();
}

void loop() {
    printValues();
    delay(delayTime);
}

void printValues() {
    float currentAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
    unsigned long currentTime = millis();
    float dt = (currentTime - prevTime) / 1000.0; // seconds

    // Calculate raw speed (m/s)
    float rawSpeed = (currentAltitude - prevAltitude) / dt;

    // Apply EMA filter
    filteredSpeed = alpha * rawSpeed + (1 - alpha) * filteredSpeed;

    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(currentAltitude);
    Serial.println(" m");

    Serial.print("Vertical Speed (EMA) = ");
    Serial.print(filteredSpeed);
    Serial.println(" m/s");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();

    // Update for next loop
    prevAltitude = currentAltitude;
    prevTime = currentTime;
}