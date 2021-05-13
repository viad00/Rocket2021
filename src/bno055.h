#include <Adafruit_BNO055.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(sensor.name);
  Serial.print("Driver Ver:   ");
  Serial.println(sensor.version);
  Serial.print("Unique ID:    ");
  Serial.println(sensor.sensor_id);
  Serial.print("Max Value:    ");
  Serial.print(sensor.max_value);
  Serial.println(" xxx");
  Serial.print("Min Value:    ");
  Serial.print(sensor.min_value);
  Serial.println(" xxx");
  Serial.print("Resolution:   ");
  Serial.print(sensor.resolution);
  Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
}

void setup_bno(void)
{
  Serial.println("Orientation Sensor Test");
  Serial.println("");

  /* Initialise the sensor */
  if (!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    return;
  }

  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(true);

  /* Display some basic information on this sensor */
  displaySensorDetails();
}

void printEvent(sensors_event_t *event, JsonObject obj)
{
  double x = -1000000, y = -1000000, z = -1000000; //dumb values, easy to spot problem
  if (event->type == SENSOR_TYPE_ACCELEROMETER)
  {
#ifdef debug
    Serial.print("Accl:");
#endif
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else if (event->type == SENSOR_TYPE_ORIENTATION)
  {
#ifdef debug
    Serial.print("Orient:");
#endif
    x = event->orientation.x;
    y = event->orientation.y;
    z = event->orientation.z;
  }
  else if (event->type == SENSOR_TYPE_MAGNETIC_FIELD)
  {
#ifdef debug
    Serial.print("Mag:");
#endif
    x = event->magnetic.x;
    y = event->magnetic.y;
    z = event->magnetic.z;
  }
  else if (event->type == SENSOR_TYPE_GYROSCOPE)
  {
#ifdef debug
    Serial.print("Gyro:");
#endif
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else if (event->type == SENSOR_TYPE_ROTATION_VECTOR)
  {
#ifdef debug
    Serial.print("Rot:");
#endif
    x = event->gyro.x;
    y = event->gyro.y;
    z = event->gyro.z;
  }
  else if (event->type == SENSOR_TYPE_LINEAR_ACCELERATION)
  {
#ifdef debug
    Serial.print("Linear:");
#endif
    x = event->acceleration.x;
    y = event->acceleration.y;
    z = event->acceleration.z;
  }
  else
  {
#ifdef debug
    Serial.print("Unk:");
#endif
  }
  obj["x"] = x;
  obj["y"] = y;
  obj["z"] = z;
#ifdef debug
  Serial.print("\tx= ");
  Serial.print(x);
  Serial.print(" |\ty= ");
  Serial.print(y);
  Serial.print(" |\tz= ");
  Serial.println(z);
#endif
}

void loop_bno(JsonObject &root)
{
  JsonObject bno055 = root.createNestedObject("bno055");
  sensors_event_t orientationData, angVelocityData, linearAccelData, magnetometerData, accelerometerData, gravityData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);
  bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);
  JsonObject orientation_data = bno055.createNestedObject("orientation_data");
  printEvent(&orientationData, orientation_data);
  JsonObject ang_velocity_data = bno055.createNestedObject("ang_velocity_data");
  printEvent(&angVelocityData, ang_velocity_data);
  JsonObject linear_accel_data = bno055.createNestedObject("linear_accel_data");
  printEvent(&linearAccelData, linear_accel_data);
  JsonObject magnetometer_data = bno055.createNestedObject("magnetometer_data");
  printEvent(&magnetometerData, magnetometer_data);
  JsonObject accelerometer_data = bno055.createNestedObject("accelerometer_data");
  printEvent(&accelerometerData, accelerometer_data);
  JsonObject gravity_data = bno055.createNestedObject("gravity_data");
  printEvent(&gravityData, gravity_data);

  int8_t boardTemp = bno.getTemp();
#ifdef bebug
  Serial.println();
  Serial.print(F("temperature: "));
  Serial.println(boardTemp);
#endif
  bno055["temperature"] = boardTemp;

  uint8_t system, gyro, accel, mag = 0;
  bno.getCalibration(&system, &gyro, &accel, &mag);
  JsonObject calibration = bno055.createNestedObject("calibration");
  calibration["system"] = system;
  calibration["gyro"] = gyro;
  calibration["accel"] = accel;
  calibration["mag"] = mag;
#ifdef debug
  Serial.println();
  Serial.print("Calibration: Sys=");
  Serial.print(system);
  Serial.print(" Gyro=");
  Serial.print(gyro);
  Serial.print(" Accel=");
  Serial.print(accel);
  Serial.print(" Mag=");
  Serial.println(mag);

  Serial.println("--");
#endif
}