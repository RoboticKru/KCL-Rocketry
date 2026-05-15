#include <SoftwareSerial.h>
#include <TinyGPS++.h>

SoftwareSerial gpsSerial(8, 9);   // RX, TX
TinyGPSPlus gps;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println("Initializing GPS...");
}

void loop() {
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {

      // ----- DIAGNOSTIC: FIX STATUS -----
      int satCount = gps.satellites.value();
      float hdop = gps.hdop.hdop();
      float speed = gps.speed.value();

      if (satCount < 4) {
        Serial.println("NO 3D FIX: Less than 4 satellites. Values except location likely to be invalid.");
      } else if (hdop > 5) {
        Serial.println("WEAK FIX: Poor signal accuracy (HDOP > 5). Expect errors.");
      } else {
        Serial.println("GOOD FIX: 3D fix with acceptable accuracy.");
      }

      // ----- LOCATION -----
      if (gps.location.isValid()) {
        Serial.print("Latitude:  "); Serial.println(gps.location.lat(), 6);
        Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
      } else {
        Serial.println("Location: INVALID");
      }

      // ----- ALTITUDE -----
      if (gps.altitude.isValid()) {
        Serial.print("Altitude:  "); Serial.print(gps.altitude.meters());
        Serial.println(" m");
      } else {
        Serial.println("Altitude: INVALID (no 3D fix)");
      }

      // ----- SPEED -----
      if (gps.speed.isValid()) {
        Serial.print("Speed:     "); Serial.print(gps.speed.kmph());
        Serial.println(" km/h");
      }

      // ----- COURSE (HEADING) ----- This is the direction of movement from the previous position
      if (gps.course.isValid()) {
        if (speed < 1.0) {
          Serial.println("Course:    INVALID (Stationary)");
        } else {
          Serial.print("Course:    "); Serial.print(gps.course.deg(), 2);
          Serial.print("° (");
          Serial.print(TinyGPSPlus::cardinal(gps.course.deg()));
          Serial.println(")");
        }
      }

      // ----- DATE & TIME -----
      if (/*gps.date.isValid() && */gps.time.isValid()) {
        /* - Dont Need the Date
        Serial.print("Date:      ");
        Serial.print(gps.date.day()); Serial.print("/");
        Serial.print(gps.date.month()); Serial.print("/");
        Serial.println(gps.date.year());
        
        This is for the time which can be good but it updates every 3 seconds
        */ 
        Serial.print("Time (UTC): ");
        Serial.print(gps.time.hour() + 1); Serial.print(":"); // Need to change the 1hr because UK Summertime is BST (UTC +1)
        Serial.print(gps.time.minute()); Serial.print(":");
        Serial.println(gps.time.second());
      } else {
        Serial.println("Time: INVALID (waiting for fix)");
      }

      // ----- SATELLITES & HDOP -----
      Serial.print("Satellites: "); Serial.println(satCount);
      Serial.print("HDOP:       "); Serial.println(hdop);

      // ----- STALE DATA CHECK ----- If any of the values are outdated by 2 seconds it will notify us
      String staleFields = "";
      unsigned long ageThreshold = 2000; // 2 seconds

      if (gps.location.age() > ageThreshold) staleFields += "Location ";
      if (gps.altitude.age() > ageThreshold) staleFields += "Altitude ";
      if (gps.speed.age() > ageThreshold) staleFields += "Speed ";
      if (gps.course.age() > ageThreshold) staleFields += "Course ";
      if (gps.time.age() > ageThreshold) staleFields += "Time ";

      if (staleFields.length() > 0) {
        Serial.print("⚠ STALE DATA: "); 
        Serial.print(staleFields); 
      }

      Serial.println("-------------------------------");
      delay(1000); // refresh every 1s
    }
  }
}
