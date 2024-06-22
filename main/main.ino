#include <Arduino.h>
#include <ArduinoJson.h>
#include "modules/temperatureSensor/temperature.h"
#include "modules/photoresistorSensor/ldr.h"
#include "modules/thermistorSensor/thermistor.h"
#include "modules/infraredSensor/infrared.h"
#include "modules/voltageSensor/voltageSensor.h"
#include "modules/currentSensor/currentSensor.h"
#include "modules/relayClass/relay.h"

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
        : relay1(Relay(2, false)), relay2(Relay(3, false)), relay3(Relay(4, false)), relay4(Relay(5, false)),
          socketRelay(Relay(6, false)), invtobatRelay(Relay(7, false)), paneltoinvRelay(Relay(8, false)),
          temperatureValue(0), thermistorValue(0), humidityValue(0), brightnessValue(0),
          windValue(0), currentValue(0), voltageValue(0)
    {
    }
};

Data systemData;
bool loopRunning = true;
bool receivingCommand = false;

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
    while (loopRunning)
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
        delay(1000);

        // Send JSON data to ESP
        Serial.println(JSONData);

        if (Serial.available() > 0)
        {
            String command = Serial.readStringUntil('\n');

            if (command == "NewTask")
            {
                loopRunning = false;         // Stop main loop
                Serial.println("CommandOK"); // Send OK to ESP
                receivingCommand = true;
                delay(1000); // Wait for ESP to prepare data
            }

            if (receivingCommand)
            {
                if (Serial.available() > 0)
                {
                    String receivedData = Serial.readStringUntil('\n');
                    StaticJsonDocument<1024> doc;
                    DeserializationError error = deserializeJson(doc, receivedData);

                    if (!error)
                    {
                        // Process received data from ESP
                        // For simplicity, assume the received data is a single integer representing a relay number
                        int relayNumber = doc["relay"].as<int>();
                        Serial.print("Received relay number from ESP: ");
                        Serial.println(relayNumber);

                        // Toggle the corresponding relay state
                        switch (relayNumber)
                        {
                        case 1:
                            systemData.relay1.setState(!systemData.relay1.getState());
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
                        default:
                            break;
                        }

                        // After processing, resume normal loop operation
                        loopRunning = true;
                        receivingCommand = false;
                    }
                }
            }
        }
    }
}
