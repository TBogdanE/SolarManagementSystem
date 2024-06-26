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
    float thermistorValue;
    float humidityValue;
    float brightnessValue;
    float windValue;
    float currentValue;
    float voltageValue;

    Data()
        : relay1(Relay(2, true)), relay2(Relay(3, true)), relay3(Relay(4, true)), relay4(Relay(5, true)),
          socketRelay(Relay(8, true)), invtobatRelay(Relay(7, true)), paneltoinvRelay(Relay(8, true)),
          temperatureValue(0), thermistorValue(0), humidityValue(0), brightnessValue(0),
          windValue(0), currentValue(0), voltageValue(0)
    {
    }
};

Data systemData;
bool loopRunning = true;
bool receivingCommand = false;

void sendData()
{
    systemData.temperatureValue = tempSensor.readTemperature();
    systemData.humidityValue = tempSensor.readHumidity();
    systemData.brightnessValue = ldrSensor.getLDRPercentage();
    systemData.thermistorValue = thermSensor.readTemperature();
    systemData.voltageValue = vSensor.getVoltage();
    systemData.currentValue = aSensor.getCurrent();
    systemData.windValue = irSensor.getWindSpeed();

    // Create a JSON document of size 1024 bytes
    StaticJsonDocument<1024> dataDocument;
    dataDocument.clear();

    // Serialize the Data object to the JSON document
    dataDocument["relay1"] = systemData.relay1.getState();
    dataDocument["relay2"] = systemData.relay2.getState();
    dataDocument["relay3"] = systemData.relay3.getState();
    dataDocument["relay4"] = systemData.relay4.getState();
    dataDocument["socketRelay"] = systemData.socketRelay.getState();
    dataDocument["invtobatRelay"] = systemData.invtobatRelay.getState();
    dataDocument["paneltoinvRelay"] = systemData.paneltoinvRelay.getState();
    dataDocument["temperatureValue"] = systemData.temperatureValue;
    dataDocument["thermistorValue"] = systemData.thermistorValue;
    dataDocument["humidityValue"] = systemData.humidityValue;
    dataDocument["brightnessValue"] = systemData.brightnessValue;
    dataDocument["windValue"] = systemData.windValue;
    dataDocument["currentValue"] = systemData.currentValue;
    dataDocument["voltageValue"] = systemData.voltageValue;

    String JSONData;
    serializeJson(dataDocument, JSONData); // Serialize dataDocument into JSONData
    Serial.println(JSONData);
    delay(1000);
}

void powerManagement()
{
}

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
    systemData.socketRelay.setState(true);
}

void loop()
{
    if (Serial.available() > 0)
    {
        String command = Serial.readStringUntil('\n');
        command.trim();
        if (command == "NewTask")
        {
            loopRunning = false;
            Serial.println("CommandOK");
            receivingCommand = true;
            delay(1000);

            while (receivingCommand)
            {
                if (Serial.available() > 0)
                {
                    String recievedData = Serial.readStringUntil('\n');
                    delay(300);
                    StaticJsonDocument<2048> doc;
                    DeserializationError error = deserializeJson(doc, recievedData);
                    if (!error)
                    {
                        int relayNumber = doc["relay"].as<int>();
                        // Serial.println(relayNumber);
                        // String cmd = doc["command"].as<String>();
                        // Serial.println("cmd " + cmd);
                        delay(300);
                        toggleRelays(relayNumber);
                        loopRunning = true;
                        receivingCommand = false;
                    }
                }
            }
        }
    }
    static unsigned long lastSendTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastSendTime >= 5000) // Adjust interval as needed (e.g., every 5 seconds)
    {
        sendData(); // Send sensor data and relay states to phone
        lastSendTime = currentTime;
    }
}

void toggleRelays(int relayNum)
{
    switch (relayNum)
    {
    case 1:
        systemData.relay1.setState(!systemData.relay1.getState());
        Serial.println(systemData.relay1.getState());
        break;
    case 2:
        systemData.relay2.setState(!systemData.relay2.getState());
        break;
    case 3:
        systemData.relay3.setState(!systemData.relay3.getState());
        break;
    case 4:
        systemData.relay4.setState(!systemData.relay4.getState());
        break;
    case 5:
        systemData.socketRelay.setState(!systemData.socketRelay.getState());
        Serial.println(systemData.socketRelay.getState());
        break;
    default:
        break;
    }
}
