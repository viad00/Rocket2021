#define SEALEVELPRESSURE_HPA (1013.25)
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include "sdcard.h"
#include "wifi_setup.h"
#include "bmp388.h"
#include "bmp180.h"
#include "bno055.h"
#include "sht31.h"
#include "gps.h"
#include "lora.h"

#define PWM 34

bool deployed = 0;

void deploy() {
  digitalWrite(PWM, HIGH);
  delay(1000);
  digitalWrite(PWM, LOW);
  delay(1000);
}

void TaskDeploycode( void * pvParameters ){
    deploy();
    deployed = 1;
}
TaskHandle_t TaskDelpoy;

float original_height;
bool flight = false;
unsigned long start = 0;
File file;
bool save = false;
float previous_height;
bool altitude = false;

void setup(){
    Serial.begin(115200);
    Wire.setClock(800000);
    setup_sdcard();
    setup_bmp();
    //setup_180();
    setup_bno();
    setup_sht();
    setup_gps();
    setup_lora();
    Serial.println();
    Serial.println("Configuring access point...");

    connectToWiFi(networkName, networkPswd);
    ESP_ERROR_CHECK(esp_wifi_set_protocol (WIFI_IF_STA, WIFI_PROTOCOL_11B));
    wifi_country_t country_info = {"JP", 1, 14, WIFI_COUNTRY_POLICY_MANUAL};
	ESP_ERROR_CHECK(esp_wifi_set_country(&country_info));
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(80));
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    pinMode(PWM, OUTPUT);

    uint8_t buffer[50] = "test.txt";
    while(1) {
        if (connected) {
    bmp.performReading();
    original_height = bmp.readAltitude(SEALEVELPRESSURE_HPA);
            udp.parsePacket();
            if(udp.read(buffer, 50) > 0){
                Serial.print("Server to client: ");
                Serial.println((char *)buffer);
                start = millis();
                break;
            }
        }
        delay(100);
        loop_lora_pre();
    }
    file = SD_MMC.open((char *)buffer, FILE_WRITE);
    previous_height = original_height;
    Serial.println(original_height);
}

void loop(){
    DynamicJsonDocument doc(1024);
    JsonObject obj = doc.to<JsonObject>();
    obj["seconds"] = millis()/1000;
    obj["deployed"] = deployed;
    obj["save"] = save;
    obj["flight"] = flight;
    obj["altitude"] = altitude;
    loop_bmp(obj);
    loop_bno(obj);
    loop_sht(obj);
    loop_gps(obj);
    loop_lora();
    serializeJson(doc, file);
    file.print('\n');
    file.flush();
    //only send data when connected
    if(connected){
        //Send a packet
        udp.beginPacket(udpAddress,udpPort);
        serializeJson(doc, udp);
        udp.println();
        udp.endPacket();
    }
    if (!save & !deployed & ((obj["bmp388"]["altitude"].as<float>() - original_height) > 50.0)) {
        Serial.println("Deploy is save");
        save = true;
    }
    if (save & !deployed & (((obj["bmp388"]["altitude"].as<float>() - previous_height) * 5) < 3.0)) {
        Serial.println("Start deploy altitude");
        altitude = true;
        deployed = true;
        digitalWrite(PWM, HIGH);
    }
    if (!flight & (max(max(abs(obj["bno055"]["linear_accel_data"]["x"].as<float>()),
        abs(obj["bno055"]["linear_accel_data"]["y"].as<float>())),
        max(abs(obj["bno055"]["linear_accel_data"]["y"].as<float>()),
        abs(obj["bno055"]["linear_accel_data"]["z"].as<float>()))) > 5.0)) {
        Serial.println("Timer trigger");
        start = millis();
        flight = true;
    }
    if(flight & ((millis()-start) > 8*1000) & !deployed) { // Вот таймер
        Serial.println("Start deploy timeout");
        deployed = 1;
        digitalWrite(PWM, HIGH);
        //xTaskCreate(TaskDeploycode,"TaskDeploy",10000,NULL,1,&TaskDelpoy);
    }
    //Wait for 1 second
    previous_height = obj["bmp388"]["altitude"].as<float>();
    delay(200);
}
