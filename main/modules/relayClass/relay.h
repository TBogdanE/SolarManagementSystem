#ifndef RELAY_H
#define RELAY_H

class Relay
{
public:
    int analogPin;
    bool status;

    Relay(int pin = 0, bool stat = false);

    void initialize();
    void setState(bool state);
    bool getState();
};

#endif // RELAY_H
