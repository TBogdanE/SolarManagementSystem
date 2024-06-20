#include "temperature.h"
#include <DHT.h>

TemperatureSensor::TemperatureSensor(int pin, int type) : _pin(pin), _type(type), _dht(pin, type) {}

void TemperatureSensor::begin()
{
    _dht.begin();
}

float TemperatureSensor::readTemperature()
{
    return _dht.readTemperature();
}

float TemperatureSensor::readHumidity()
{
    return _dht.readHumidity();
}
