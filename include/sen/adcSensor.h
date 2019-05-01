/**
 * Generic class for adc sensor
 * Author: Ivan Moreno
 * March 2019
 */
#ifndef ADCSENSOR_H_
#define ADCSENSOR_H_

class adcSensor{
public:
    adcSensor(){ /*Nothing*/}
    ~adcSensor(){ /*Nothing*/}
    virtual double getVoltage(){  return 0;   }
    virtual void setConfigValues(){ /*Nothing*/    }

};

#endif //ADCSENSOR_H_