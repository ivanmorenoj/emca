#include <stdio.h>

#include "flagManager.h"
#include "AlphasenseSensor.h"
#include "MCP3424.h"
#include "bme280.h"

#define BME280_ADRR     0x77

struct MCP3424Config weCfg = {
    .address        = 0x69,
    .bitrate        = 18,
    .gain           = 1,
    .conv_mode      = 1,
    .channel        = 1
};
struct MCP3424Config aeCfg = {
    .address        = 0x69,
    .bitrate        = 18,
    .gain           = 1,
    .conv_mode      = 1,
    .channel        = 2
};
struct alphasenseConfig gasCfg = {
  .sensor           = CO_SENSOR,
  .compensate       = CO_B4,
  .algorithm        = 2,
  .we_electronic    = 0.277,
  .we_total         = 0.275,
  .ae_electronic    = 0.282,
  .ae_total         = 0.275,
  .sensitivity      = 0.489
};
struct operatingConditions cnd = {
    .lifeTime   = 10000,     
    .startupTime= time(NULL),     
    .temperature= {-20.0,50.0},   
    .pressure   = {0.0,1000},      
    .humidity   = {0.0,90.0},      
    .ranges     = {0.0,900.0}        
};
double adjust(double vi,unsigned char opt){
    return  2.488132904 * vi;
}
int main(int argc, char const *argv[])
{
    bme280 amb(BME280_ADRR);

    MCP3424 adc[2];
    adc->setConfigValues(&weCfg);
    (adc+1)->setConfigValues(&aeCfg);

    AlphasenseSensor gas;
    gas.setConfigValues(gasCfg,&amb,adc,adc+1);
    gas.setAdjustFunct(&adjust);

    flagManager _fm(&cnd,&amb,&gas);

    printf("Ambiental:\n\tTemperatura: %f \n\tHumedad: %f \n\tPresion:: %f \n\t\n",
            _fm.getAmbientalSensor()->getTemperature(),
            _fm.getAmbientalSensor()->getHumidity(),
            _fm.getAmbientalSensor()->getPressure());

    printf("Gas %s :\n\tWE electrode: %f\n\tAE electrode: %f\n\tPPB: %f\n\tFLAG: %s\n\t",
            _fm.getGasSensor()->getSensorName(),
            _fm.getGasSensor()->getAdc(1)->getVoltage(),
            _fm.getGasSensor()->getAdc()->getVoltage(),
            _fm.getGasSensor()->getConcentration(1,0),
            _fm.getFlag());
    printf("End of the program\n");
    return 0;
}
