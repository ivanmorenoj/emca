/**
 * Generic class for adc sensor
 * Author: Ivan Moreno
 * March 2019
 */
#ifndef ADCSENSOR_H_
#define ADCSENSOR_H_

#include <time.h>

#define _INTERVAL_ADCS    5  //seg
class adcSensor{
public:
    adcSensor():_lastTime(0){ /*Nothing*/}
    ~adcSensor(){ /*Nothing*/}
    virtual double getVoltage(){  return 0;   }
    virtual void setConfigValues(){ /*Nothing*/    }

private:
    time_t _lastTime;
    
protected:
    bool isTime() {
        if (time(NULL) - _lastTime > _INTERVAL_ADCS) {
            _lastTime = time(NULL);
            return true;
        }
        return false;
    }
};

#endif //ADCSENSOR_H_