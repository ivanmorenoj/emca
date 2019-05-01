#include <stdio.h>
#include <time.h>
#include "AlphasenseSensor.h"
#include "MCP3424.h"
#include "bme280.h"

#define BME280_ADRR     0x77

struct MCP3424Config weCfg = {
    .address        = 0x69,
    .bitrate        = 18,
    .gain           = 1,
    .conv_mode = 1,
    .channel        = 1
};
struct MCP3424Config aeCfg = {
    .address        = 0x69,
    .bitrate        = 18,
    .gain           = 1,
    .conv_mode = 1,
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

double adjust(double vi,unsigned char opt){
    return  2.488132904 * vi;
}

int main(int argc, char const *argv[])
{
    bme280 amb(BME280_ADRR);

    MCP3424 adc[2];
    adc[0].setConfigValues(&weCfg);
    adc[1].setConfigValues(&aeCfg);

    AlphasenseSensor gas(gasCfg,&amb,adc,adc+1);
    gas.setAdjustFunct(&adjust);

    printf("The sensor name is %s \n",gas.getSensorName());
    printf("The gas consentration corrected by temepratue is \n\t%.4f ppm\n\t%.4f ppb\n\n",
            gas.getConcentration(),gas.getConcentration(1,0));

    printf("The gas consentration uncorrected\n\t%.4f ppm\n\t%.4f ppb\n",
        gas.getConcentration(0,1),gas.getConcentration(1,1));


    printf("adc sensor we: %f\nadc sensor ae: %f\n",
            gas.getAdc(1)->getVoltage(),gas.getAdc()->getVoltage());

    printf("End of the program\n");
    
    return 0;
}
