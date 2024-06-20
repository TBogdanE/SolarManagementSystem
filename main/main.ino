#include <Arduino.h>
#include <ArduinoJson.h>
#include "modules/temperatureSensor/temperature.h"
#include "modules/temperatureSensor/temperature.cpp"
#include "modules/photoresistorSensor/ldr.h"
#include "modules/photoresistorSensor/ldr.cpp"
#include "modules/thermistorSensor/thermistor.h"
#include "modules/thermistorSensor/thermistor.cpp"
#include "modules/infraredSensor/infrared.h"
#include "modules/infraredSensor/infrared.cpp"
#include "modules/voltageSensor/voltageSensor.h"
#include "modules/voltageSensor/voltageSensor.cpp"
#include "modules/currentSensor/currentSensor.h"
#include "modules/currentSensor/currentSensor.cpp"
#include "modules/relayClass/relay.h"
#include "modules/relayClass/relay.cpp"

#define DHT_PIN 9
#define DHT_TYPE DHT11
#define LDR_PIN A1
#define THER_PIN A3
#define IR_PIN A4
#define V_PIN A0
#define CURR_PIN A5

TemperatureSensor tempSensor(DHT_PIN, DHT_TYPE);
LDRSensor ldrSensor(LDR_PIN);
ThermistorSensor thermSensor(THER_PIN);
InfraredSensor irSensor(IR_PIN);
VoltageSensor vSensor(V_PIN);
CurrentSensor aSensor(CURR_PIN);

class Data
{

public:
    Relay relay1;
    Relay relay2;
    Relay relay3;
    Relay relay4;
    Relay socketRelay;
    Relay invtobatRelay;
    Relay paneltoinvRelay;
    float temperatureValue;
    float humidityValue;
    float brightnessValue;
    float windValue;
    float currentValue;
    float voltageValue;
    Data() : relay1(Relay(2, false)), relay2(Relay(3, false)), relay3(Relay(4, false)), relay4(Relay(5, false)),
             socketRelay(Relay(6, false)), invtobatRelay(Relay(7, false)), paneltoinvRelay(Relay(8, false)), temperatureValue(0),
             humidityValue(0), brightnessValue(0), windValue(0), currentValue(0), voltageValue(0) {}
};

Data systemData;
int i = 1;

void setup()
{
    Serial.begin(9600);
    tempSensor.begin();
    systemData.relay1.initialize();
    systemData.relay2.initialize();
    systemData.relay3.initialize();
    systemData.relay4.initialize();
    systemData.socketRelay.initialize();
    systemData.invtobatRelay.initialize();
    systemData.paneltoinvRelay.initialize();
}
void loop()
{

    systemData.temperatureValue = tempSensor.readTemperature();
    systemData.humidityValue = tempSensor.readHumidity();
    systemData.brightnessValue = ldrSensor.getLDRPercentage();
    systemData.brightnessValue = thermSensor.readTemperature();
    systemData.brightnessValue = thermSensor.readTemperature();
    systemData.voltageValue = vSensor.getVoltage();
    systemData.currentValue = aSensor.getCurrent();
    Serial.println(systemData.currentValue);

    // Create a JSON document of size 1024 bytes
    StaticJsonDocument<1024> doc;

    // Serialize the Data object to the JSON document
    doc["relay1"] = systemData.relay1.getState();
    doc["relay2"] = systemData.relay2.getState();
    doc["relay3"] = systemData.relay3.getState();
    doc["relay4"] = systemData.relay4.getState();
    doc["socketRelay"] = systemData.socketRelay.getState();
    doc["invtobatRelay"] = systemData.invtobatRelay.getState();
    doc["paneltoinvRelay"] = systemData.paneltoinvRelay.getState();
    doc["temperatureValue"] = systemData.temperatureValue;
    doc["humidityValue"] = systemData.humidityValue;
    doc["brightnessValue"] = systemData.brightnessValue;
    doc["windValue"] = systemData.windValue;
    doc["currentValue"] = systemData.currentValue;
    doc["voltageValue"] = systemData.voltageValue;

    Serial.println(i);
    i = i + 2;

    /*
       // Print the JSON document to the serial monitor
       serializeJson(doc, Serial);

       // Send the JSON document via the RX/TX pins to the ESP
       Serial.println();

      Serial.print("Temperature: ");
       Serial.print(systemData.temperatureValue);
       Serial.print(" °C, Humidity: ");
       Serial.print(systemData.humidityValue);
       Serial.println(" %");
       Serial.print("Temperature: ");
       Serial.print(systemData.brightnessValue);
       Serial.println(" °C");
       Serial.print(irSensor.getWindSpeed());
       Serial.print(systemData.voltageValue);
       */

    delay(1000); // Adjust delay as needed
}
