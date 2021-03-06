// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include "SSD1306Wire.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
SSD1306Wire display(0x3C, 0, 2, GEOMETRY_128_32);


// To do
// Include in wifimanager, options to specify POST interval, server ip, port, enable 1 or both sensors, enable or disable screen
// Have screen cycle through temps, ip, next update interval


// Data wire is plugged into port 2 on the ESP8266
#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer = { 0x28, 0xFF, 0x3B, 0x87, 0x73, 0x16, 0x05, 0x91 }; // garage
DeviceAddress outsideThermometer = { 0x28, 0xFF, 0x66, 0x9C, 0x74, 0x16, 0x04, 0x60 }; // attic
//DeviceAddress insideThermometer = { 0x28, 0xFF, 0xC6, 0xB5, 0x70, 0x16, 0x05, 0x70 }; // intake for AC
//DeviceAddress outsideThermometer = { 0x28, 0xFF, 0x81, 0x98, 0x74, 0x16, 0x04, 0xE5 }; // output for AC


void setup(void)
{
  Wire.pins(0, 2);
  Wire.begin(0, 2);

  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.clear();
  display.drawString(0, 0, "Pending Config");
  display.display();


  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 8, "WiFi Connected!");
  display.display();

  // Start up the library
  sensors.begin();

  // set the resolution to 9 bit per device
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);
}


void loop()
{
  sensors.requestTemperatures();
  float Sensor1TempC = sensors.getTempC(insideThermometer);
  float Sensor1TempF = DallasTemperature::toFahrenheit(Sensor1TempC);
  float Sensor2TempC = sensors.getTempC(outsideThermometer);
  float Sensor2TempF = DallasTemperature::toFahrenheit(Sensor2TempC);

  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Sensor 1: " + String(Sensor1TempF));
  display.drawString(0, 16, "Sensor 2: " + String(Sensor2TempF));
  display.display();


  if (true) {
    const uint16_t port = 8082;
    const char* host = "172.16.20.17";
    WiFiClient client;

    String PostData = "{ node=drop ";
    PostData += " ";
    PostData += "Sensor1=";
    PostData += Sensor1TempF;
    PostData += " ";
    PostData += " ";
    PostData += "Sensor2=";
    PostData += Sensor2TempF;
    PostData += " }";
    if (client.connect(host, port)) {
      client.println(PostData);
      client.stop();
    }
  }
  delay(60000);
}
