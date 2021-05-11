#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp180;

void setup_180() {
    if (!bmp180.begin()) {
	    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    }
}

void loop_180(JsonObject &root) {
    JsonObject bmp388 = root.createNestedObject("bmp388");
    /*
    Serial.print("Temperature = ");
    Serial.print(bmp180.readTemperature());
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(bmp180.readPressure());
    Serial.println(" Pa");
    
    // Calculate altitude assuming 'standard' barometric
    // pressure of 1013.25 millibar = 101325 Pascal
    Serial.print("Altitude = ");
    Serial.print(bmp180.readAltitude());
    Serial.println(" meters");

    Serial.print("Pressure at sealevel (calculated) = ");
    Serial.print(bmp180.readSealevelPressure());
    Serial.println(" Pa");

  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp180.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" meters");
    
    Serial.println();
    */
    bmp388["temperature"] = bmp180.readTemperature();
    bmp388["pressure"] = bmp180.readPressure();
    bmp388["altitude"] = bmp180.readAltitude();
}