#include <SPI.h>
#include <SX127XLT.h>

#define NSS 5                                   //select on LoRa device
#define SCK 18                                  //SCK on SPI3
#define MISO 19                                 //MISO on SPI3 
#define MOSI 23                                 //MOSI on SPI3 

#define NRESET 35                               //reset on LoRa device
#define DIO0 33                                 //DIO0 on LoRa device, used for RX and TX done 
#define DIO1 32                                 //DIO1 on LoRa device, normally not used so set to -1
#define DIO2 -1                                 //DIO2 on LoRa device, normally not used so set to -1

#define LORA_DEVICE DEVICE_SX1278                //this is the device we are using

//*******  Setup LoRa Test Parameters Here ! ***************

//LoRa Modem Parameters
const uint32_t Frequency = 429700000;           //frequency of transmissions
const uint32_t Offset = 0;                      //offset frequency for calibration purposes

const uint8_t Bandwidth = LORA_BW_062;          //LoRa bandwidth
const uint8_t SpreadingFactor = LORA_SF12;      //LoRa spreading factor
const uint8_t CodeRate = LORA_CR_4_8;           //LoRa coding rate
const uint8_t Optimisation = LDRO_AUTO;         //low data rate optimisation setting

const int8_t TXpower = 20;                      //LoRa transmit power in dBm

SX127XLT LT;

void setup_lora() {
    SPI.begin(SCK, MISO, MOSI, NSS);
if (LT.begin(NSS, NRESET, DIO0, DIO1, DIO2, LORA_DEVICE))
  {
    Serial.println(F("LoRa Device found"));
  }
  else
  {
    Serial.println(F("No device responding"));
  }

  LT.setupLoRa(Frequency, Offset, SpreadingFactor, Bandwidth, CodeRate, Optimisation);

  Serial.println();
  LT.printModemSettings();                         //reads and prints the configured LoRa settings, useful check
  Serial.println();
  LT.printOperatingSettings();                    //reads and prints the configured operating settings, useful check
  Serial.println();
    LT.startWriteSXBuffer(0);                   //initialise buffer write at address 0
  LT.writeFloat(0.0);                         //add latitude
  LT.writeFloat(0.0);                         //add longitude
  LT.writeFloat(0.0);                         //add altitude
  LT.writeUint8(0);                      //add number of satellites
  LT.writeUint8(0);                    //add tracker status
  uint8_t len = LT.endWriteSXBuffer();                //close buffer write

  LT.transmitSXBuffer(0, len, 10000, TXpower, WAIT_TX);
}

void sendLocation(int32_t Lat, int32_t Lon)
{
  uint8_t len;

  LT.startWriteSXBuffer(0);                   //initialise buffer write at address 0
  LT.writeInt32(Lat);                         //add latitude
  LT.writeInt32(Lon);                         //add longitude
  LT.writeInt16(fix.alt.whole);                         //add altitude
  LT.writeInt8(fix.alt.frac);
  LT.writeUint8(fix.satellites);                      //add number of satellites
  LT.writeUint8(fix.status);                    //add tracker status
  len = LT.endWriteSXBuffer();                //close buffer write
  /*
  Serial.print("Sending data: ");
  Serial.print(fix.alt.whole);
  Serial.print(".");
  Serial.println(fix.alt.frac);
  */
  LT.transmitSXBuffer(0, len, 10000, TXpower, NO_WAIT);
}


void loop_lora_pre() {
  if (gps.available( gpsPort) && digitalRead(DIO0)) {
    fix = gps.read();
    sendLocation(fix.latitudeL(), fix.longitudeL());
  }
}

void loop_lora() {
  if (digitalRead(DIO0)) {
    sendLocation(fix.latitudeL(), fix.longitudeL());
  }
}
