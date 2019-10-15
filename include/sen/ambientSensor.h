/**
 * Generic class for ambient sensor
 * Author: Ivan Moreno
 * March 2019
 */
#ifndef AMBIENTSENSOR_H_
#define AMBIENTSENSOR_H_

#include <time.h>
#include <stdint.h>

#define _INTERVAL_AS    60  //seg

class ambientSensor{
public:
    ambientSensor():_lastTime(0){  /*Nothing*/    }
    ~ambientSensor(){  /*Nothing*/    }
    virtual float getTemperature(){ return 0;   }
    virtual float getPressure(){    return 0;   }
    virtual float getHumidity(){    return 0;   }
private:
    time_t _lastTime;
    
protected:
    bool isTime() {
        if (time(NULL) - _lastTime > _INTERVAL_AS) {
            _lastTime = time(NULL);
            return true;
        }
        return false;
    }
};

#endif //AMBIENTSENSOR_H_

