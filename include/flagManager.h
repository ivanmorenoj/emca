#ifndef FLAG_MANAGER_H_
#define FLAG_MANAGER_H_

#include <time.h>
#include <stdint.h>
#include "commonConfig.h"
#include "sen/gasSensor.h"
#include "sen/ambientSensor.h"


class flagManager{
public:
    flagManager();
    flagManager(struct operatingConditions *cnd,ambientSensor *am,gasSensor *gasS);
    ~flagManager();
    void setConfig(struct operatingConditions *cnd,ambientSensor *am,gasSensor *gasS);
    void setOperiatingConditions(struct operatingConditions *cnd);
    const char *getFlag();
    gasSensor *getGasSensor();
    ambientSensor *getAmbientalSensor();

private:
    struct operatingConditions *cond;
    gasSensor *gas;
    ambientSensor *amb;
private:
    /**
     *  return: 
     *      0: is in range
     *      1: temperature is out of range
     *      2: pressure is out of range
     *      3: humidity is out of range
     */
    uint8_t checkAmbientConditions();
    /**
     *  return: 
     *      0: is out of range
     *      1: is in range
     */
    uint8_t checkLife();
    /**
     *  return: 
     *      0: is in range
     *      1: is below the range
     *      2: is above the range
     *      3: is in the range close to zero
     */
    uint8_t checkRanges();
};

#endif //FLAG_MANAGER_H_