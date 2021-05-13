#include <NMEAGPS.h>
#ifndef GPSport_h
#define GPSport_h

#define gpsPort Serial2
#define GPS_PORT_NAME "Serial2"

#endif

static NMEAGPS gps;
gps_fix fix;

void setup_gps()
{
  gpsPort.begin(9600);
}

void loop_gps(JsonObject &root)
{
  if (gps.available(gpsPort))
  {
    fix = gps.read();
  }
  JsonObject gps = root.createNestedObject("gps");
  gps["lat"] = fix.latitude();
  gps["lon"] = fix.longitude();
  gps["alt"] = fix.altitude();
}