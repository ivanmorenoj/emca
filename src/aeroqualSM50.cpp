#include "aeroqualSM50.h"

aeroqualSM50::aeroqualSM50() {
    adc = NULL;
}
aeroqualSM50::aeroqualSM50(ambientSensor *am,adcSensor *ad) {
    setConfigValues(am,ad);
}
aeroqualSM50::~aeroqualSM50() {
    /* Nothing */
}
float aeroqualSM50::getConcentration(char type,char nused) {
    /**
     * From DAC output
     * 5v   =   10ppm
     * Nv   =   Xppm
     * Xppm =    (Nv * 10ppm) / 5v 
     * Xppm =   Nv * 2 [ppm]
     */
    double con = 0;
    con = adc->getVoltage() * 2.0; // [ppm]

    if (type){
        con = con * 1000.0;   // [ppb]
    } 
    
    return (float)con;
}
void aeroqualSM50::setConfigValues(ambientSensor *am,adcSensor *ad) {
    adc = ad;
    amb = am;
}
char *aeroqualSM50::getSensorName() {
    return (char *)"O3";
}
adcSensor *aeroqualSM50::getAdc(int t) {
    return adc;
}
ambientSensor *aeroqualSM50::getAmbient() {
    return amb;
}