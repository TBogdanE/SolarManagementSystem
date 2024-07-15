// NEEDS ARDUINO CLI INSTALLED AND CONFIGURED

// include Arduino.h becasue we are working in VS Code, not Arduino IDE
#include <Arduino.h>
#include <ArduinoJson.h>
// include our modules
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
#include "modules/windSensor/windSensor.h"
#include "modules/windSensor/windSensor.cpp"

// define pins for the sensors
#define DHT_PIN 9
#define DHT_TYPE DHT11
#define LDR_PIN A1
#define THER_PIN A3
#define IR_PIN A4
#define V_PIN A0
#define W_PIN 10
#define CURR_PIN A5

// setting global values
#define DAY_THRESHOLD 500           // LDR sensor threshold for detecting day/night
#define MAX_BATTERY_VOLTAGE 14.8    // Maximum battery voltage when fully charged
#define MAX_SAFE_TEMP 60            // Maximum safe temperature for the solar panel (in degrees Celsius)
#define MAX_SAFE_WIND_SPEED 50      // Maximum safe wind speed (in km/h)
#define MAX_SOLAR_INPUT 115         // Maximum solar input (in watts)
#define HIGH_CONSUMER_THRESHOLD 250 // Power draw threshold to consider turning on high consumers (in watts)
#define CRITICAL_TEMP 30            // Critical temperature threshold (in degrees Celsius)
#define CRITICAL_HUMIDITY 70        // Critical humidity threshold (in percentage)

// classes for sensor objects in ./modules
TemperatureSensor tempSensor(DHT_PIN, DHT_TYPE);
LDRSensor ldrSensor(LDR_PIN);
ThermistorSensor thermSensor(THER_PIN);
InfraredSensor irSensor(IR_PIN);
VoltageSensor vSensor(V_PIN);
CurrentSensor aSensor(CURR_PIN);
WindSensor wSensor(W_PIN);

// data object for storing everything we need to send over to phone
// also, we are creating objects for each relay
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
    float batteryPercentage;

    Data()
        : relay1(Relay(2, true)), relay2(Relay(3, true)), relay3(Relay(4, true)), relay4(Relay(5, true)),
          socketRelay(Relay(8, true)), invtobatRelay(Relay(7, true)), paneltoinvRelay(Relay(8, true)),
          temperatureValue(0), thermistorValue(0), humidityValue(0), brightnessValue(0),
          windValue(0), currentValue(0), voltageValue(0), batteryPercentage(0)
    {
    }
};

Data systemData;
bool loopRunning = true;
bool receivingCommand = false;

void sendData()
{
    // read data from all the sensors
    systemData.temperatureValue = tempSensor.readTemperature();
    systemData.humidityValue = tempSensor.readHumidity();
    systemData.brightnessValue = ldrSensor.getLDRLux();
    systemData.thermistorValue = thermSensor.readTemperature();
    systemData.voltageValue = vSensor.getVoltage();
    systemData.batteryPercentage = vSensor.getBatteryPercentage();
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
    dataDocument["humidityValue"] = systemData.humidityValue;
    dataDocument["thermistorValue"] = systemData.thermistorValue;
    dataDocument["brightnessValue"] = systemData.brightnessValue;
    dataDocument["windValue"] = systemData.windValue;
    dataDocument["batteryPercentage"] = systemData.batteryPercentage;
    dataDocument["currentValue"] = systemData.currentValue;
    dataDocument["voltageValue"] = systemData.voltageValue;

    String JSONData;
    serializeJson(dataDocument, JSONData); // Serialize dataDocument into JSONData
    Serial.println(JSONData);
    delay(1000);
}

// logic for consumers management
void manageConsumers()
{
    // check if day or night
    bool is_daytime = (systemData.brightnessValue > DAY_THRESHOLD);

    if (is_daytime)
    {
        if (systemData.batteryPercentage > 60)
        {
            turnOnAllConsumers();
        }
        else if (systemData.batteryPercentage > 40)
        {
            turnOnLowConsumers();
            evaluateHighConsumersBasedOnSolarInputAndPowerDraw();
        }
        else
        {
            turnOffHighConsumers();
            if (criticalLowConsumersNeeded())
            {
                turnOnLowConsumers();
            }
        }
    }
    else
    {
        if (systemData.batteryPercentage > 70)
        {
            turnOnAllConsumers();
        }
        else if (systemData.batteryPercentage > 50)
        {
            turnOnLowConsumers();
            turnOffHighConsumers();
        }
        else
        {
            turnOffHighConsumers();
            if (criticalLowConsumersNeeded())
            {
                turnOnLowConsumers();
            }
            else
            {
                turnOffAllConsumers();
            }
        }
    }

    // safety checks
    if (systemData.thermistorValue > MAX_SAFE_TEMP)
    {
        reduceLoadToPreventOverheating();
    }
    if (systemData.windValue > MAX_SAFE_WIND_SPEED)
    {
        disconnectSolarPanelForSafety();
        switchToBatteryOnlyMode();
    }
}

void turnOnAllConsumers()
{
    systemData.relay1.setState(true);
    systemData.relay2.setState(true);
    systemData.relay3.setState(true);
    systemData.relay4.setState(true);
}

void turnOnLowConsumers()
{
    systemData.relay1.setState(true);
    systemData.relay2.setState(true);
}

void evaluateHighConsumersBasedOnSolarInputAndPowerDraw()
{
    // check if the current solar input is sufficient to handle high consumers
    float currentPowerDraw = systemData.currentValue * systemData.voltageValue;
    float remainingCapacity = MAX_SOLAR_INPUT - currentPowerDraw;

    if (remainingCapacity > HIGH_CONSUMER_THRESHOLD)
    {
        systemData.relay3.setState(true);
        systemData.relay4.setState(true);
    }
    else
    {
        systemData.relay3.setState(false);
        systemData.relay4.setState(false);
    }
}

bool criticalLowConsumersNeeded()
{
    // determine if critical low consumers are needed based on temperature and humidity
    if (systemData.temperatureValue > CRITICAL_TEMP || systemData.humidityValue > CRITICAL_HUMIDITY)
    {
        return true;
    }
    return false;
}

void reduceLoadToPreventOverheating()
{
    // Reduce load by turning off high consumers
    systemData.relay3.setState(false);
    systemData.relay4.setState(false);
}

void disconnectSolarPanelForSafety()
{
    systemData.paneltoinvRelay.setState(false);
}

void switchToBatteryOnlyMode()
{
    systemData.relay1.setState(true);
    systemData.relay2.setState(true);
    systemData.relay3.setState(false);
    systemData.relay4.setState(false);
}

void turnOffAllConsumers()
{
    systemData.relay1.setState(false);
    systemData.relay2.setState(false);
    systemData.relay3.setState(false);
    systemData.relay4.setState(false);
}

void turnOffHighConsumers()
{
    systemData.relay3.setState(false);
    systemData.relay4.setState(false);
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
    // everytime the loop starts, it will check if is recieving on serial a specific command we need, in our case, 'NewTask'
    //  if we recieve this command, we will stop the loop, send back via serial to ESP 'CommandOK', so we tell ESP we are ready
    //  to recieve the final command; after, we are waiting for the command, and execute it, than, go back to normal work
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
                        delay(300);
                        // toggle specific relay selected from phone
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
    if (currentTime - lastSendTime >= 5000)
    {
        sendData(); // send sensor data and relay states to phone
        lastSendTime = currentTime;
    }
}

// function that toggles relay state
// we recieve a relay number, we than change it's state to it's opposite state, and toggle the relay
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
    case 6:
        systemData.invtobatRelay.setState(!systemData.invtobatRelay.getState());
        Serial.println(systemData.invtobatRelay.getState());
        break;
    case 7:
        systemData.paneltoinvRelay.setState(!systemData.paneltoinvRelay.getState());
        Serial.println(systemData.paneltoinvRelay.getState());
        break;
    default:
        break;
    }
}
