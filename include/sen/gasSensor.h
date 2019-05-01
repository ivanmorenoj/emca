/**
 * Generic class for gas sensor
 * Author: Ivan Moreno
 * March 2019
 */
#ifndef GASSENSOR_H_
#define GASSENSOR_H_

#include <time.h>
#include <stdint.h>
#include "adcSensor.h"
#include "ambientSensor.h"

#define _INTERVAL_GS   60

class gasSensor{
public:
    gasSensor():adjustFunc(NULL),_lastTime(0){    /*Nothing*/ }
    ~gasSensor(){   /*Nothing*/ }
    virtual float getConcentration(char a=0,char b=0){   return 0;  }
    virtual void setConfigValues(){ /*Nothing*/ }
    virtual char *getSensorName(){  return NULL;   }
    virtual adcSensor *getAdc(int t=0){ return NULL;}
    virtual ambientSensor *getAmbient(){ return NULL;}
    void setAdjustFunct(double (*Func)(double,unsigned char)){ adjustFunc = Func; }
protected:
    /**
     *  return the adjust voltaje by funct
     *  @param vi - voltaje intput 
     *  @param opt - switch over different inputs if the sensor has it
     */
    double (*adjustFunc)(double vi,unsigned char opt);
    bool isTime() {
        if (time(NULL) - _lastTime > _INTERVAL_GS) {
            _lastTime = time(NULL);
            return true;
        }
        return false;
    }

private:
    time_t _lastTime;
};

#endif //GASSENSOR_H_