#ifndef AEROQUALSM50_H_
#define AEROQUALSM50_H_

#include <cstddef>
#include "sen/gasSensor.h"
#include "sen/adcSensor.h"
#include "sen/ambientSensor.h"

class aeroqualSM50: public gasSensor{
public:
    aeroqualSM50();
    aeroqualSM50(ambientSensor *am,adcSensor *ad);
    ~aeroqualSM50();
    float getConcentration(char type=0,char nused=0);
    void setConfigValues(ambientSensor *am,adcSensor *ad);
    char *getSensorName();
    adcSensor *getAdc(int t=0);
    ambientSensor *getAmbient();
private:
    adcSensor *adc;
    ambientSensor *amb;
};


#endif //AEROQUALSM50_H_