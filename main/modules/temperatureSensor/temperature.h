#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <DHT.h> // Include the DHT library

class TemperatureSensor
{
public:
    TemperatureSensor(int pin, int type);
    void begin();
    float readTemperature();
    float readHumidity();

private:
    int _pin;
    int _type;
    DHT _dht;
};

#endif
