#include <Adafruit_BMP3XX.h>

Adafruit_BMP3XX bmp;

void setup_bmp() {
  if (!bmp.begin_I2C()) {   // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    return;
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop_bmp(JsonObject &root) {
JsonObject bmp388 = root.createNestedObject("bmp388");
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  #ifdef debug
  Serial.print("Temperature = ");
  Serial.print(bmp.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bmp.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");
  
  Serial.println();
  #endif
  bmp388["temperature"] = bmp.temperature;
  bmp388["pressure"] = bmp.pressure;
  bmp388["altitude"] = bmp.readAltitude(SEALEVELPRESSURE_HPA);
}