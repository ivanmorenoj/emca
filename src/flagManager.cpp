#include "flagManager.h"

flagManager::flagManager() {    
    cond = NULL;
    gas = NULL;
    amb = NULL;
}
flagManager::flagManager(struct operatingConditions *cnd,ambientSensor *am,gasSensor *gasS) {
    setConfig(cnd,am,gasS);
}
flagManager::~flagManager() {
    /* Nothing */
}
void flagManager::setConfig(struct operatingConditions *cnd,ambientSensor *am,gasSensor *gasS) {
    cond = cnd;
    gas = gasS;
    amb = am;
}
void flagManager::setOperiatingConditions(struct operatingConditions *cnd) {
    cond = cnd;    
}
const char *flagManager::getFlag() {
    /**
     * Banderas para el manejo de datos
     *  -VA:  Valido  
     *  -IC:  Invalido por calibracion 
     *  -IR:  Invalido por rango de operacion
     *  -VZ:  Valido igualado a cero o al limite de deteccion
     *  ND:  No disponible
     *  -IF:  Invalido por falla del equipo de monitoreo
     */
    uint8_t ambient_f   = checkAmbientConditions();
    uint8_t life_f      = checkLife();
    uint8_t rangues     = checkRanges(); 

    if (ambient_f)
        return "IR";
    else if (!life_f)
        return "IC";
    else if (rangues > 0 && rangues < 3)
        return "IR";
    else if (!life_f && rangues>0 && rangues<3)
        return "IF";
    else if (!ambient_f && life_f && rangues == 3)
        return "VZ";
    else if (!ambient_f && life_f && !rangues)
        return "VA";
    else if(ambient_f && !life_f && rangues)
        return "ND";
    return NULL;
}
uint8_t flagManager::checkAmbientConditions() {
    char aux = 0;
    float tmp = amb->getTemperature();
    float pre = amb->getPressure();
    float hum = amb->getHumidity();

    if (tmp < cond->temperature[0] || tmp > cond->temperature[1])
        aux = 1;
    else if (pre < cond->pressure[0] || pre > cond->pressure[1])
        aux = 2;
    else if (hum < cond->humidity[0] || hum > cond->humidity[1])
        aux = 3;

    return aux;
}
uint8_t flagManager::checkLife() {
    time_t real_time = time(NULL) - cond->startupTime;

    if(real_time > cond->lifeTime)
        return 0;
    else 
        return 1;
}
uint8_t flagManager::checkRanges() {
    uint8_t aux = 0;
    static const double zRanges[2] = {cond->ranges[0],(0.05)*(cond->ranges[1])};
    
    float gasCon = gas->getConcentration(1,0);
    
    if (gasCon < cond->ranges[0])
        aux = 1;
    else if (gasCon > cond->ranges[1])
        aux = 2;
    else if (gasCon > zRanges[0] && gasCon < zRanges[1])
        aux = 3;

    return aux;
}
gasSensor *flagManager::getGasSensor() {
    return gas;
}
ambientSensor *flagManager::getAmbientalSensor() {
    return amb;
}
