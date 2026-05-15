// If the SD Card does not initialise, you will need to format the SD card to FAT32 Format --> https://guiformat.com/

/* To reset the formatting of SD Card follow these steps:

On Windows:
Open File Explorer → Right-click on the SD card → Select Format.
In the dialog:
File System: Select exFAT
Allocation size: Leave as Default
Check Quick Format
Click Start.

On macOS:
Open Disk Utility
Select the SD card in the left pane.
Click Erase
Format: exFAT
Scheme: GUID Partition Map (or Master Boot Record)
Click Erase.
*/

#include <SPI.h> //for the SD card module
#include <SD.h> // for the SD card
#include <dht.h> // for the DHT sensor


int pinDHT11 = A0;  // Set the pin connected to DATA in the DHT11 sensor
int DHT11Temperature;  // Variable to store the value of the temperature
int DHT11Humidity;  // Variable to store the value of the Humidity
int chk;  // Create variable to store raw reading from the DHT11 sensor

char buffer[64]; // Adjust size if needed
float timestamp = 0.0;

const int chipSelect = 10; // Where the CS wire is connected to

// Create a file to store the data
File dataFile;

dht DHT;  // Create sensor

void setup() {
  //initializing Serial monitor
  Serial.begin(9600); 
    
  // setup for the SD card
  Serial.print("Initializing SD card...");

  if(!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  // This was used to make sure I am able to write to SD Card  
  // createFile("test.txt");
  // writeToFile("This is to test the module!");
  // closeFile();
}

void loop() {
   loggingData();// This continually logs data onto the SD Card
}

void loggingData() {

dataFile = SD.open("log.txt", FILE_WRITE);
if (dataFile) {
  dataFile.print("Time: ");
  dataFile.print(timestamp, 1); // Print float with 1 decimal place
  dataFile.print("s - Value: ");
  loggingTemperature();
  dataFile.close();
  timestamp += 0.1; // 100ms delay between each reading, this can be reduced if needed
  delay(100);
}
}

void loggingTemperature() {
  chk = DHT.read11(pinDHT11);  // Read the raw data from the sensor
  DHT11Temperature = DHT.temperature;  // Store the value of the temperature
  DHT11Humidity = DHT.humidity;  // Store the value of the Humidity
  
  // Check if any reads failed and exit early (to try again).
  if  (isnan(DHT11Temperature) || isnan(DHT11Humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  //debugging purposes
  Serial.print("Temperature:");  // Print the text "Temperature: " in the serial monitor
  Serial.print(DHT11Temperature);  // Print the value inside the variable called Temperature
  // Serial.print(" C");  // Print the text " C" in the serial monitor
  Serial.print(",");  // Print a tab in the serial monitor

  Serial.print("Humidity:");  // Print the text "Humidity: " in the serial monitor
  Serial.print(DHT11Humidity);  // Print the value inside the variable called Humidity
  // Serial.print(" %");  // Print the text " %" in the serial monitor
  Serial.println();  // Create a new line in the serial monitor

  snprintf(buffer, sizeof(buffer), "Temperature: %d C, Humidity: %d %%", DHT11Temperature, DHT11Humidity);
  writeToFile(buffer);


}


int createFile(char filename[])
{
  dataFile = SD.open(filename, FILE_WRITE);

  if (dataFile)
  {
    Serial.println("File created successfully.");
    return 1;
  } else
  {
    Serial.println("Error while creating file.");
    return 0;
  }
}

int writeToFile(char text[])
{
  if (dataFile)
  {
    dataFile.println(text);
    Serial.println("Writing to file: ");
    Serial.println(text);
    return 1;
  } else
  {
    Serial.println("Couldn't write to file");
    return 0;
  }
}

void closeFile()
{
  if (dataFile)
  {
    dataFile.close();
    Serial.println("File closed");
  }
}

int openFile(char filename[])
{
  dataFile = SD.open(filename);
  if (dataFile)
  {
    Serial.println("File opened with success!");
    return 1;
  } else
  {
    Serial.println("Error opening file...");
    return 0;
  }
}
