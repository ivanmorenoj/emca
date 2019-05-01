#include <stdio.h>
#include "aeroqualSM50.h"
#include "MCP3424.h"
#include "bme280.h"

struct MCP3424Config aeroqualADC = {
    .address        = 0x6A,
    .bitrate        = 18,
    .gain           = 1,
    .conv_mode      = 1,
    .channel        = 3
};

int main(int argc, char const *argv[])
{
    bme280 _amb(0x77);
    MCP3424 _adc;
    _adc.setConfigValues(&aeroqualADC); 
    aeroqualSM50 o3sensor(&_amb,&_adc);

    printf("voltage : %f\n",_adc.getVoltage());
    printf("Sensor name: %s\n",o3sensor.getSensorName());
    printf("Concentration: \n\t%.4f ppm\n\t%.4f ppb\n",o3sensor.getConcentration(),o3sensor.getConcentration(1));

    printf("The adc intput is: %f\n",o3sensor.getAdc()->getVoltage());
    printf("End of the program\n");
    return 0;
}
